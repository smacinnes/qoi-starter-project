# Tests

If you want to add unit tests, put them under this folder.
The project is already configured to use the Catch2 testing framework.
Take a look at `test/qoi/sum.cpp` to get a sense of the boilerplate you'll need.
You can learn more about the Catch2 testing framework [here](TODO).

A good convention to follow is to mirror the file/directory structure from src/.
This makes it easy to find the tests associated with specific modules.
Any test files you create should be added to `TEST_SOURCES` in `test/CMakeLists.txt`.

The Catch2 testing framework is a C++ library.
All test files should therefore be compiled by a C++ compiler.
CMake will sort this out for us, as long as the files have a .cpp extension.
Note that in our test source files, we wrap the `#include`s with `extern "C"` so the linker can make sense of the C/C++ mix.
