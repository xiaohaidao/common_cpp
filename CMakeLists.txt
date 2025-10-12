cmake_minimum_required(VERSION 3.14)

project("common_cpp" VERSION 0.2.1)

# import common configure
include(cmake/get_cmake.cmake)

add_dir_src(src)

add_dir_tests(tests)
