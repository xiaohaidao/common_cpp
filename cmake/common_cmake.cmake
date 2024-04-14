
include(FetchContent)
FetchContent_Declare(
    common_cmake
    GIT_REPOSITORY https://github.com/xiaohaidao/common_cmake.git
    GIT_TAG dev
    GIT_SHALLOW ON
    GIT_PROGRESS ON
)
FetchContent_MakeAvailable(common_cmake)

configure_file(${CMAKE_CURRENT_LIST_DIR}/dependencies.cmake cmake/dependencies_${PROJECT_NAME}.cmake)
configure_file(${CMAKE_CURRENT_LIST_DIR}/dependencies_dev.cmake cmake/dependencies_dev_${PROJECT_NAME}.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}/cmake/")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${common_cmake_SOURCE_DIR}/cmake")
# list(APPEND CMAKE_MODULE_PATH "${FETCHCONTENT_BASE_DIR}/common_cmake-src/cmake")

#import tool
include(common)
include(msvc_static_runtime)
