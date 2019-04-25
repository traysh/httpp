cmake_minimum_required (VERSION 2.6)
project (httpp)
# The version number.
set (httpp_VERSION_MAJOR 0)
set (httpp_VERSION_MINOR 1)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/httppConfig.h.in"
  "${PROJECT_BINARY_DIR}/httppConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find httppConfig.h
include_directories("${PROJECT_BINARY_DIR}")
 
# add the executable
add_executable(httpp httpp.cpp)
