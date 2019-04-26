cmake_minimum_required (VERSION 3.14)
project (httpp)
# The version number.
set (httpp_VERSION_MAJOR 0)
set (httpp_VERSION_MINOR 1)

# For use by YouCompleteMe
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Compile flags
SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fexceptions -std=c++11 \
    -isystem /usr/include \
    -isystem /usr/local/include \
    -isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/c++/v1 \
    -isystem /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include")
SET(GCC_COVERAGE_LINK_FLAGS "-lgcov")

 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/httppConfig.h.in"
  "${PROJECT_BINARY_DIR}/httppConfig.h"
  )

# Search for headers directories
file(GLOB_RECURSE includes CONFIGURE_DEPENDS "*.hpp")
foreach(header ${includes})
    get_filename_component(dir "${header}" DIRECTORY)
    list(APPEND include_dirs ${dir})
endforeach()
list(REMOVE_DUPLICATES include_dirs)
message("include dirs = ${include_dirs}")

# add the binary tree to the search path for include files
# so that we will find httppConfig.h
include_directories("${PROJECT_BINARY_DIR}" "${include_dirs}")

# Whenever this glob's value changes, cmake will rerun and update the build with the
# new/removed files.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")

# add the executable
add_executable(httpp ${sources})
