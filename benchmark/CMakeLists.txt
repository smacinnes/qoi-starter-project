find_package(Catch2 3 REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

set(QOI_BENCHMARK_REFERENCE_FILEPATH "${PROJECT_SOURCE_DIR}/data/benchmark/reference.bin")
set(QOI_BENCHMARK_TEST_FILEPATH_0 "${PROJECT_SOURCE_DIR}/data/benchmark/0.qoi")
set(QOI_BENCHMARK_TEST_FILEPATH_1 "${PROJECT_SOURCE_DIR}/data/benchmark/1.qoi")
set(QOI_BENCHMARK_TEST_FILEPATH_2 "${PROJECT_SOURCE_DIR}/data/benchmark/2.qoi")
set(QOI_BENCHMARK_TEST_FILEPATH_3 "${PROJECT_SOURCE_DIR}/data/benchmark/3.qoi")

configure_file(benchmark.in.cpp benchmark.cpp @ONLY)

set(TEST_SOURCES benchmark.cpp)

add_executable(qoi_benchmark ${TEST_SOURCES})
target_link_libraries(
  qoi_benchmark
  PRIVATE Catch2::Catch2WithMain
          qoi
          qoi_options
          SDL2::SDL2
          SDL2::SDL2_image
)

include(CTest)
include(Catch)
catch_discover_tests(qoi_benchmark)
