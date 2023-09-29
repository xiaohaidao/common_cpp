
include(FetchContent)
FetchContent_Declare(
    common_cmake
    GIT_REPOSITORY https://github.com/xiaohaidao/common_cmake.git
    GIT_TAG dev
    GIT_SHALLOW ON
    GIT_PROGRESS ON
)
FetchContent_MakeAvailable(common_cmake)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${common_cmake_SOURCE_DIR}/cmake")
# list(APPEND CMAKE_MODULE_PATH "${FETCHCONTENT_BASE_DIR}/common_cmake-src/cmake")

#import tool
include(common)
