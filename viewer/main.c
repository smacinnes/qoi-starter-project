#include "placeholder.h"

#include "qoi/qoi.h"

#include "SDL.h"
#include "SDL_image.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 600

void print_usage() {
  puts("usage: qoi_viewer FILE");
  puts("  FILE: path to QOI file to open");
}

void *load_file(char const *filename, uint64_t *out_size) {
  FILE *file = fopen(filename, "rb");
  long file_size = 0;

  if (!file) {
    puts("error: failed to open file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);

  if (file_size < 0) {
    puts("error: failed to determine file size");
    fclose(file);
    return NULL;
  } else if (file_size == 0) {
    puts("error: empty file\n");
    fclose(file);
    return NULL;
  }

  fseek(file, 0, SEEK_SET);

  void *data = malloc((size_t)file_size);
  if (!data) {
    puts("error: failed to allocate memory for input image");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(data, 1, (size_t)file_size, file);
  if (bytes_read != (size_t)file_size) {
    if (feof(file)) {
      puts("error: reached end of file earlier than expected");
    }
    int read_error = ferror(file);
    if (read_error) {
      printf("error: an error occurred while reading file: %d\n", read_error);
    }
    fclose(file);
    return NULL;
  }

  fclose(file);
  *out_size = (uint64_t)bytes_read;

  return data;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("error: 1 argument expected (%d provided)\n", argc - 1);
    print_usage();
    return EXIT_FAILURE;
  }

  if (SDL_Init(SDL_INIT_VIDEO)) {
    printf("failed to initialize SDL: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  char const *filename = argv[1];

  printf("opening %s\n", filename);

  uint64_t size = 0;
  void *data = load_file(filename, &size);
  if (!data) {
    puts("failed to read file into memory");
    return EXIT_FAILURE;
  }

  qoi_desc_t desc = {0};
  void *pixels = qoi_decode(data, size, &desc);
  free(data);

  if (!pixels) {
    puts("failed to decode image");
  } else if (desc.width > (uint32_t)INT32_MAX || desc.height > (uint32_t)INT32_MAX) {
    printf("image too large to be rendered: %" PRIu32 " x %" PRIu32 "\n", desc.width, desc.height);
    free(pixels);
    pixels = NULL;
  }

  SDL_Surface *surface = NULL;
  if (!pixels) {
    // Open the placeholder image
    SDL_RWops *rw = SDL_RWFromConstMem((void const *)placeholder, (int)placeholder_len);
    surface = IMG_LoadPNG_RW(rw);
    SDL_RWclose(rw);
    // Override the image description
    desc.width = (uint32_t)surface->w;
    desc.height = (uint32_t)surface->h;
  } else {
    surface =
        SDL_CreateRGBSurfaceFrom(pixels, (int)desc.width, (int)desc.height, 32, 4 * (int)desc.width,
                                 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
  }

  float aspect_ratio = (float)desc.width / (float)desc.height;
  int window_height = (int)((float)WINDOW_WIDTH / aspect_ratio);

  SDL_Window *window =
      SDL_CreateWindow(filename, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                       window_height, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  uint32_t window_id = SDL_GetWindowID(window);

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  bool should_loop = true;
  while (should_loop) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_WINDOWEVENT:
          if (event.window.windowID == window_id) {
            switch (event.window.event) {
              case SDL_WINDOWEVENT_CLOSE:
                event.type = SDL_QUIT;
                SDL_PushEvent(&event);
                break;
            }
          }
          break;
        case SDL_QUIT:
          should_loop = false;
          break;
      }
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(10);
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  free(pixels);
  return EXIT_SUCCESS;
}
