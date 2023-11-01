
option(ENABLE_URL_PREFIX "enable url prefix" OFF)
if(ENABLE_URL_PREFIX)
    if(NOT GITHUB_URL_PREFIX)
        set(GITHUB_URL_PREFIX https://gh.api.99988866.xyz/) # https://gh.api.99988866.xyz  https://ghproxy.com/ https://toolwa.com/github/
    endif()
else()
    set(GITHUB_URL_PREFIX "")
endif()
message("GITHUB_URL_PREFIX: ${GITHUB_URL_PREFIX}, ENABLE_URL_PREFIX: ${ENABLE_URL_PREFIX}")

include(FetchContent)
FetchContent_Declare(
    common_cmake
    GIT_REPOSITORY ${GITHUB_URL_PREFIX}https://github.com/xiaohaidao/common_cmake.git
    GIT_TAG dev
    GIT_SHALLOW ON
    GIT_PROGRESS ON
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(common_cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${common_cmake_SOURCE_DIR}/cmake")
# list(APPEND CMAKE_MODULE_PATH "${FETCHCONTENT_BASE_DIR}/common_cmake-src/cmake")

#import tool
include(common)
