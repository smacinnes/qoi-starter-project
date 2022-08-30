set(QOI_PUBLIC_HEADERS qoi/qoi.h)

set(QOI_SOURCES qoi/details/unused.h qoi/qoi.c)

add_library(qoi STATIC)

# Add source files
target_sources(qoi PRIVATE ${QOI_SOURCES})
target_sources(qoi PUBLIC FILE_SET HEADERS FILES ${QOI_PUBLIC_HEADERS})

target_include_directories(
  qoi PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
             $<INSTALL_INTERFACE:include>
)

target_link_libraries(qoi PRIVATE qoi_options qoi_warnings)
