find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_executable(qoi_viewer main.c placeholder.h)

target_link_libraries(
  qoi_viewer
  PRIVATE qoi
          qoi_options
          qoi_warnings
          SDL2::SDL2
          SDL2::SDL2main
          SDL2::SDL2_image
)
