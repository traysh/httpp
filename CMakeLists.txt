cmake_minimum_required (VERSION 3.14)

project (httpp)
set (httpp_VERSION_MAJOR 0)
set (httpp_VERSION_MINOR 1)

# For use by YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fexceptions -Wfatal-errors")
set(CMAKE_CXX_LINK_FLAGS "-fprofile-arcs -ftest-coverage")

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
add_library(${CMAKE_PROJECT_NAME}.so EXCLUDE_FROM_ALL ${sources} ${includes})
# generate code coverage data
target_compile_options(${CMAKE_PROJECT_NAME}.so PRIVATE -O0 -g -fprofile-arcs -ftest-coverage)
target_link_options(${CMAKE_PROJECT_NAME}.so PRIVATE -fprofile-arcs -ftest-coverage)

################################
# GTest
################################
add_subdirectory (googletest EXCLUDE_FROM_ALL)

# Unit Tests
add_executable(unit EXCLUDE_FROM_ALL src/test_socket.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(unit ${CMAKE_PROJECT_NAME}.so gtest gtest_main)
# generate code coverage data
target_compile_options(unit PRIVATE -O0 -g -fprofile-arcs -ftest-coverage)
target_link_options(unit PRIVATE -fprofile-arcs -ftest-coverage)

add_custom_target(test-unit 
                  COMMAND unit
                  COMMAND gcovr -r ${PROJECT_SOURCE_DIR} -sdp --exclude-directories 'googletest|build'
                                --filter ${PROJECT_SOURCE_DIR}/src --exclude '\.\./.*/test_.*'
                  DEPENDS unit)

add_custom_target(test DEPENDS test-unit)
