# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE
  )
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo"
  )
endif()

# Set a debug postfix if none was specified
if(NOT CMAKE_DEBUG_POSTFIX)
  message(STATUS "Setting debug postfix to 'd' as none was specified.")
  set(CMAKE_DEBUG_POSTFIX
      d
      CACHE STRING "Choose the postfix for debug artifacts." FORCE
  )
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Turn on folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT
   (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    AND CMAKE_LIBRARY_OUTPUT_DIRECTORY
    AND CMAKE_RUNTIME_OUTPUT_DIRECTORY)
)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
  message(STATUS "Setting archive output directory to ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
  message(STATUS "Setting library output directory to ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
  message(STATUS "Setting runtime output directory to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
