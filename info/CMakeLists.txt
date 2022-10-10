add_executable(qoi_info main.c)

target_link_libraries(
  qoi_info
  PRIVATE qoi
          qoi_options
          qoi_warnings
)
