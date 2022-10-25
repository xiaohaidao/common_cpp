# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

cmake_minimum_required(VERSION 3.14)

project("your_project_name")

# import common configure
include(cmake/common.cmake)

# import code configure
include(clang_format)
include(clang_tidy)

add_subdirectory(src)
# add_subdirectory(example)

option(BUILD_TESTING "Build test" OFF)
include(CTest)
if(BUILD_TESTING)
    include(googletest)

    ADD_SUBDIRECTORY(tests)
endif()

