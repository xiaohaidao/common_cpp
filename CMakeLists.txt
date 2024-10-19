cmake_minimum_required(VERSION 3.14)

project("common_cpp" VERSION 0.2.1)

# import common configure
include(cmake/get_cmake.cmake)

# import configure
include(CPackSourceConfig)

# import dependencies
include(dependencies_${PROJECT_NAME})

add_subdirectory(src)

option(BUILD_TESTING "Build test" OFF)
include(CTest)
if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    include(dependencies_dev_${PROJECT_NAME})

    add_subdirectory(tests)
endif()
