cmake_minimum_required (VERSION 3.14)

project (httpp)
set (httpp_VERSION_MAJOR 0)
set (httpp_VERSION_MINOR 1)

# For use by YouCompleteMe
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fexceptions -std=c++14")
SET(GCC_COVERAGE_LINK_FLAGS "--coverage")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/httppConfig.h.in"
  "${PROJECT_BINARY_DIR}/httppConfig.h"
  )

# Search for headers directories
file(GLOB_RECURSE includes CONFIGURE_DEPENDS "src/*.hpp")
foreach(header ${includes})
    get_filename_component(dir "${header}" DIRECTORY)
    list(APPEND include_dirs ${dir})
endforeach()
list(REMOVE_DUPLICATES include_dirs)

include_directories("${PROJECT_BINARY_DIR}" "${include_dirs}")

# Whenever this glob's value changes, cmake will rerun and update the build with the
# new/removed files.
set (main "${PROJECT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME}.cpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER sources EXCLUDE REGEX "test_.*")
list(REMOVE_ITEM sources "${main}")

# add the executables
add_executable(${CMAKE_PROJECT_NAME} ${main}  ${sources} ${includes})
add_library(${CMAKE_PROJECT_NAME}.so ${sources} ${includes})

################################
# GTest
################################
ADD_SUBDIRECTORY (googletest)
enable_testing()

# Unit Tests
add_executable(test_unit src/test_socket.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(test_unit ${CMAKE_PROJECT_NAME}.so gtest gtest_main)

add_test(test-unit test_unit)
