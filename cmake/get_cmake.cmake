set(get_cmake_default_version 0.1.0)
if(get_cmake_version)
    message(
        "-- get_cmake versoin: ${get_cmake_version}, default version: ${get_cmake_default_version}"
    )
else()
    set(get_cmake_version ${get_cmake_default_version})
    message("-- set get_cmake default versoin: ${get_cmake_version}")
endif()

if(NOT GIT_COMMON_CMAKE_REPOSITORY)
    set(GIT_COMMON_CMAKE_REPOSITORY
        https://github.com/xiaohaidao/common_cmake.git)
endif()
if(NOT COMMON_CMAKE_VERSION)
    set(COMMON_CMAKE_VERSION dev)
endif()

if(NOT COMMAND fetch_add_packet)
    macro(fetch_add_packet_macro name)
        # set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
        # set(FETCHCONTENT_FULLY_DISCONNECTED ON)
        include(FetchContent)
        FetchContent_Declare(
            ${name}
            ${ARGN} OVERRIDE_FIND_PACKAGE
            GIT_SHALLOW ON
            GIT_PROGRESS ON)
        FetchContent_MakeAvailable(${name})
    endmacro()

    function(fetch_add_packet)
        fetch_add_packet_macro(${ARGV})
    endfunction()
endif()

if(NOT CUSTOM_LOCAL_COMMON_CMAKE)
    if((NOT ${CMAKE_PROJECT_NAME} STREQUAL "workspace_project")
       AND ((EXISTS ${CMAKE_SOURCE_DIR}/../common_cmake)
            OR (EXISTS ${PROJECT_SOURCE_DIR}/../common_cmake)))
        if(EXISTS ${CMAKE_SOURCE_DIR}/../common_cmake)
            fetch_add_packet_macro(common_cmake SOURCE_DIR
                                   ${CMAKE_SOURCE_DIR}/../common_cmake)
        else()
            fetch_add_packet_macro(common_cmake SOURCE_DIR
                                   ${PROJECT_SOURCE_DIR}/../common_cmake)
        endif()
    else()
        fetch_add_packet_macro(
            common_cmake GIT_REPOSITORY ${GIT_COMMON_CMAKE_REPOSITORY} GIT_TAG
            ${COMMON_CMAKE_VERSION})
    endif()
else()
    fetch_add_packet_macro(common_cmake SOURCE_DIR ${CUSTOM_LOCAL_COMMON_CMAKE})
endif()
list(APPEND CMAKE_MODULE_PATH "${common_cmake_SOURCE_DIR}/cmake")
# import include
include(include_cmake)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}/cmake")
