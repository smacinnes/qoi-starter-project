cmake_minimum_required(VERSION 3.23)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file"
)

project(
  qoi
  VERSION 1.0.1
  LANGUAGES C CXX
)

# Configure global settings
include(cmake/StandardProjectSettings.cmake)

# Add an interface library add target properties
add_library(qoi_options INTERFACE)
target_compile_features(qoi_options INTERFACE c_std_11)
target_compile_features(qoi_options INTERFACE cxx_std_17)

# Add another interface library for managing warnings
add_library(qoi_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(qoi_warnings)

# Define some options to configure the build
option(BUILD_DOCS "Generate API docs with Doxygen" ON)
option(BUILD_TESTS "Build and configure units tests with Catch2" ON)
option(BUILD_IMAGE_VIEWER
       "Build basic image viewer to visually inspect decoded images" ON
)
option(BUILD_BENCHMARK "Build benchmark test" ON)

# Only support fuzzing with Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  option(BUILD_FUZZ_TESTS "Build fuzz test with libfuzzer" OFF)
endif()

message(STATUS "Doxygen generation: ${BUILD_DOCS}")
message(STATUS "Unit tests: ${BUILD_TESTS}")
message(STATUS "Viewer: ${BUILD_IMAGE_VIEWER}")
message(STATUS "Fuzz tests: ${BUILD_FUZZ_TESTS}")

# Enable address and undefined behaviour sanitizers when fuzzing
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND BUILD_FUZZ_TESTS)
  target_compile_options(
    qoi_options INTERFACE -fsanitize=address,undefined --coverage -O0 -g
  )
  target_link_options(
    qoi_options INTERFACE -fsanitize=address,undefined --coverage
  )
endif()

add_subdirectory(src)

if(BUILD_DOCS)
  add_subdirectory(docs)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND BUILD_FUZZ_TESTS)
  add_subdirectory(fuzz_test)
endif()

if(BUILD_IMAGE_VIEWER)
  add_subdirectory(viewer)
endif()

if(BUILD_BENCHMARK)
  add_subdirectory(benchmark)
endif()
