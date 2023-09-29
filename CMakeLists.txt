
cmake_minimum_required(VERSION 3.14)

project("common_cpp"
    VERSION 0.2.1
)

# import common configure
include(cmake/common_cmake.cmake)

# import configure
include(clang_format)
include(clang_tidy)
include(CPackSourceConfig)

add_subdirectory(src)
# add_subdirectory(example)

option(BUILD_TESTING "Build test" OFF)
include(CTest)
if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    include(googletest)

    ADD_SUBDIRECTORY(tests)
endif()

