find_package(Catch2 3 REQUIRED)

set(TEST_SOURCES qoi/sum.cpp)

add_executable(qoi_tests ${TEST_SOURCES})
target_link_libraries(qoi_tests PRIVATE Catch2::Catch2WithMain qoi qoi_options)

include(CTest)
include(Catch)
catch_discover_tests(qoi_tests)
