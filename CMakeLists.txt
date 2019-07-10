cmake_minimum_required (VERSION 3.14)

project (httpp)
set (httpp_VERSION_MAJOR 0)
set (httpp_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -fexceptions -Wfatal-errors")

find_package (Threads)

# For use by YouCompleteMe
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# Whenever this glob's value changes, cmake will rerun and update the build with
# the new/removed files.
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER sources EXCLUDE REGEX "test_.*")

# add the libraries
add_library("${CMAKE_PROJECT_NAME}_STATIC" ${sources} ${includes})
set_target_properties("${CMAKE_PROJECT_NAME}_STATIC" PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_link_libraries("${CMAKE_PROJECT_NAME}_STATIC" ${CMAKE_THREAD_LIBS_INIT})

add_library("${CMAKE_PROJECT_NAME}_SHARED" SHARED ${sources} ${includes})
set_target_properties("${CMAKE_PROJECT_NAME}_SHARED" PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_link_libraries("${CMAKE_PROJECT_NAME}_SHARED" ${CMAKE_THREAD_LIBS_INIT})

################################
# Examples
################################
file(GLOB_RECURSE examples CONFIGURE_DEPENDS "examples/*/*.cpp")

add_executable("${CMAKE_PROJECT_NAME}_EXAMPLE" EXCLUDE_FROM_ALL ${examples} ${includes})
set_target_properties("${CMAKE_PROJECT_NAME}_EXAMPLE" PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})
target_link_libraries("${CMAKE_PROJECT_NAME}_EXAMPLE" "${CMAKE_PROJECT_NAME}_SHARED")

add_custom_target(examples DEPENDS ${CMAKE_PROJECT_NAME}_EXAMPLE)

################################
# GTest
################################
option(BUILD_GMOCK "Builds Google Mock" OFF)
add_subdirectory (googletest EXCLUDE_FROM_ALL)
mark_as_advanced(BUILD_GMOCK)

# Unit Tests
file(GLOB_RECURSE tests CONFIGURE_DEPENDS "src/test_*.cpp")
add_executable(unit EXCLUDE_FROM_ALL ${tests} ${sources} ${includes})

# Link test executable against gtest & gtest_main
target_link_libraries(unit gtest)

# generate code coverage data
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using llvm
    set(GCOV_BIN "llvm-cov gcov")
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    set(GCOV_BIN "gcov")
endif()

target_compile_options(unit PRIVATE -Og -ggdb3 -fprofile-arcs -ftest-coverage -fstandalone-debug
                       -DTEST -DPROJECT_ROOT="${PROJECT_SOURCE_DIR}")
target_link_options(unit PRIVATE -ggdb3 -fprofile-arcs -ftest-coverage -fstandalone-debug)

add_custom_target(test-unit 
                  COMMAND find ${PROJECT_BINARY_DIR} -name '*.gcda'
                                                     -exec rm {} +
                  COMMAND unit
                  COMMAND gcovr -r ${PROJECT_SOURCE_DIR} -sp
                                --gcov-executable "${GCOV_BIN}"
                                --exclude-directories 'googletest|build'
                                --filter ${PROJECT_SOURCE_DIR}/src
                                --exclude '\.\./.*/test_.*|\.\./.*_prod\..*'
                  DEPENDS unit)
add_custom_target(test-debug
                  COMMAND find ${PROJECT_BINARY_DIR} -name '*.gcda'
                                                     -exec rm {} +
                  COMMAND lldb ./unit
                  DEPENDS unit)


add_custom_target(test DEPENDS test-unit)
