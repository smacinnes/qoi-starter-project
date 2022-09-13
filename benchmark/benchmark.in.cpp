extern "C" {
#include "qoi/qoi.h"
}

#include "catch2/catch_all.hpp"

#include "SDL.h"
#include "SDL_image.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

namespace {

char const *REFERENCE_FILEPATH = "@QOI_BENCHMARK_REFERENCE_FILEPATH@";
int const REFERENCE_WIDTH = 256;
int const REFERENCE_HEIGHT = 256;

char const *TEST_FILEPATH_0 = "@QOI_BENCHMARK_TEST_FILEPATH_0@";
char const *TEST_FILEPATH_1 = "@QOI_BENCHMARK_TEST_FILEPATH_1@";
char const *TEST_FILEPATH_2 = "@QOI_BENCHMARK_TEST_FILEPATH_2@";
char const *TEST_FILEPATH_3 = "@QOI_BENCHMARK_TEST_FILEPATH_3@";

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

std::string to_string(std::uint8_t value) {
  std::stringstream rss;
  rss << value;
  rss << " (0x" << std::setfill('0') << std::hex << std::setw(2)
      << static_cast<std::uint32_t>(value) << ')';

  return rss.str();
}

}  // namespace

TEST_CASE("benchmark", "[qoi_decode]") {
  auto reference_image = load_file(REFERENCE_FILEPATH);

  auto test_filepath =
      GENERATE_COPY(TEST_FILEPATH_0, TEST_FILEPATH_1, TEST_FILEPATH_2, TEST_FILEPATH_3);
  auto test_image = load_file(test_filepath);

  qoi_desc_t desc = {0};
  auto *pixels =
      static_cast<std::uint8_t const *>(qoi_decode(test_image.data(), test_image.size(), &desc));

  REQUIRE(pixels != nullptr);
  REQUIRE(desc.width == REFERENCE_WIDTH);
  REQUIRE(desc.height == REFERENCE_HEIGHT);

  for (std::size_t i = 0; i < reference_image.size(); ++i) {
    auto const test_byte = pixels[i];
    auto const ref_byte = static_cast<std::uint8_t>(reference_image[i]);

    CAPTURE(test_filepath);
    CAPTURE(i);
    INFO("ref_byte := " << to_string(ref_byte));
    INFO("test_bytes := " << to_string(test_byte));
    REQUIRE(test_byte == ref_byte);
  }
}
