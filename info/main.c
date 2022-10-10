#include "qoi/qoi.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


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
  }

  printf("image width:       %10" PRIu32 "\n", desc.width);
  printf("image height:      %10" PRIu32 "\n", desc.height);
  printf("image channels:    %10" PRIu32 "\n", desc.channels);
  printf("image colourspace: %10" PRIu32 "\n", desc.colorspace);

  printf("last byte:         0x%08x\n", ((uint8_t const *)pixels)[desc.width * desc.height * 4]);

  free(pixels);
}
