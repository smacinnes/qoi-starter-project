// This is an example test. You can use it as a template for your own unit tests.

#include "catch2/catch_test_macros.hpp"

extern "C" {
// include your header here
// #include "qoi/example.h"
}

TEST_CASE("test sum", "[sum]") {
  REQUIRE(2 == 1 + 1);
}