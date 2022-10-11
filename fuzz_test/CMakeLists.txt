add_executable(qoi_fuzz_test fuzz_test.c)
target_link_libraries(qoi_fuzz_test PRIVATE qoi)

target_compile_options(qoi_fuzz_test PRIVATE -fsanitize=fuzzer,address,undefined --coverage -O0 -g)
target_link_options(qoi_fuzz_test PRIVATE -fsanitize=fuzzer,address,undefined --coverage)
