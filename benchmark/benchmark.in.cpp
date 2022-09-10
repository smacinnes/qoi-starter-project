extern "C" {
#include "qoi/qoi.h"
}

#include "catch2/catch_all.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>

namespace {

char const *reference_filepath = "@QOI_BENCHMARK_REFERENCE_FILEPATH@";
char const *test_filepath_0 = "@QOI_BENCHMARK_TEST_FILEPATH_0@";
char const *test_filepath_1 = "@QOI_BENCHMARK_TEST_FILEPATH_1@";
char const *test_filepath_2 = "@QOI_BENCHMARK_TEST_FILEPATH_2@";
char const *test_filepath_3 = "@QOI_BENCHMARK_TEST_FILEPATH_3@";

std::vector<char> load_file(char const *filepath) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(static_cast<std::size_t>(size));
  file.read(buffer.data(), size);
  if (!file) {
    throw std::runtime_error("I/O error while reading");
  }

  return buffer;
}

std::vector<char> decode_png(char const *filepath, int *w, int *h) {
  auto surface =
      std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)>(IMG_Load(filepath), SDL_FreeSurface);
  if (!surface) {
    throw std::runtime_error("failed to decode reference image");
  }

  if (surface->pitch != surface->w * 4) {
    throw std::runtime_error("image decoded with unexpected pitch");
  }

  *w = surface->w;
  *h = surface->h;

  std::size_t buffer_size =
      static_cast<std::size_t>(surface->pitch) * static_cast<std::size_t>(surface->h);
  char const *buffer = static_cast<char const *>(surface->pixels);
  return std::vector<char>(buffer, buffer + buffer_size);
}

}  // namespace

TEST_CASE("benchmark", "[qoi_decode]") {
  int reference_w, reference_h;
  auto reference_image = decode_png(reference_filepath, &reference_w, &reference_h);

  auto test_filepath = GENERATE_COPY(test_filepath_0, test_filepath_1, test_filepath_2, test_filepath_3);
  auto test_image = load_file(test_filepath);

  qoi_desc_t desc = {0};
  void *pixels = qoi_decode(test_image.data(), test_image.size(), &desc);
  
  REQUIRE(pixels != nullptr);
  REQUIRE(desc.width == reference_w);
  REQUIRE(desc.height == reference_h);

  REQUIRE(std::memcmp(pixels, reference_image.data(), reference_image.size()) == 0);
}