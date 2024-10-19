if(NOT get_cmake_version)
    set(get_cmake_version 0.1.0)
else()
    message("common_cmake exist")
    return()
endif()

if(NOT GIT_COMMON_CMAKE_REPOSITORY)
    set(GIT_COMMON_CMAKE_REPOSITORY
        https://github.com/xiaohaidao/common_cmake.git)
endif()
if(NOT COMMON_CMAKE_VERSION)
    set(COMMON_CMAKE_VERSION dev)
endif()

# Parse the argument of CPMAddPackage in case a single one was provided and
# convert it to a list of arguments which can then be parsed idiomatically. For
# example gh:foo/bar@1.2.3 will be converted to:
# GITHUB_REPOSITORY;foo/bar;VERSION;1.2.3
function(cpm_parse_add_package_single_arg arg outArgs)
    # Look for a scheme
    if("${arg}" MATCHES "^([a-zA-Z]+):(.+)$")
        string(TOLOWER "${CMAKE_MATCH_1}" scheme)
        set(uri "${CMAKE_MATCH_2}")

        # Check for CPM-specific schemes
        if(scheme STREQUAL "gh")
            set(out "GITHUB_REPOSITORY;${uri}")
            set(packageType "git")
        elseif(scheme STREQUAL "gl")
            set(out "GITLAB_REPOSITORY;${uri}")
            set(packageType "git")
        elseif(scheme STREQUAL "bb")
            set(out "BITBUCKET_REPOSITORY;${uri}")
            set(packageType "git")
            # A CPM-specific scheme was not found. Looks like this is a generic
            # URL so try to determine type
        elseif(arg MATCHES ".git/?(@|#|$)")
            set(out "GIT_REPOSITORY;${arg}")
            set(packageType "git")
        else()
            # Fall back to a URL
            set(out "URL;${arg}")
            set(packageType "archive")

            # We could also check for SVN since FetchContent supports it, but
            # SVN is so rare these days. We just won't bother with the
            # additional complexity it will induce in this function. SVN is done
            # by multi-arg
        endif()
    else()
        if(arg MATCHES ".git/?(@|#|$)")
            set(out "GIT_REPOSITORY;${arg}")
            set(packageType "git")
        else()
            # Give up
            message(
                FATAL_ERROR
                    "${CPM_INDENT} Can't determine package type of '${arg}'")
        endif()
    endif()

    # For all packages we interpret @... as version. Only replace the last
    # occurrence. Thus URIs containing '@' can be used
    string(REGEX REPLACE "@([^@]+)$" ";VERSION;\\1" out "${out}")

    # Parse the rest according to package type
    if(packageType STREQUAL "git")
        # For git repos we interpret #... as a tag or branch or commit hash
        string(REGEX REPLACE "#([^#]+)$" ";GIT_TAG;\\1" out "${out}")
    elseif(packageType STREQUAL "archive")
        # For archives we interpret #... as a URL hash.
        string(REGEX REPLACE "#([^#]+)$" ";URL_HASH;\\1" out "${out}")
        # We don't try to parse the version if it's not provided explicitly.
        # cpm_get_version_from_url should do this at a later point
    else()
        # We should never get here. This is an assertion and hitting it means
        # there's a problem with the code above. A packageType was set, but not
        # handled by this if-else.
        message(
            FATAL_ERROR
                "${CPM_INDENT} Unsupported package type '${packageType}' of '${arg}'"
        )
    endif()

    set(${outArgs}
        ${out}
        PARENT_SCOPE)
endfunction()

# include(FetchContent) FetchContent_Declare( common_cmake GIT_REPOSITORY
# ${GIT_COMMON_CMAKE_REPOSITORY} GIT_TAG ${COMMON_CMAKE_VERSION} GIT_SHALLOW ON
# GIT_PROGRESS ON ) FetchContent_MakeAvailable(common_cmake)
macro(fetch_add_packet_macro name)
    include(FetchContent)
    FetchContent_Declare(
        ${name}
        ${ARGN}
        GIT_SHALLOW ON
        GIT_PROGRESS ON)
    string(TOUPPER ${name} names)
    set(FETCHCONTENT_UPDATES_DISCONNECTED_${names} ON)
    # or set(FETCHCONTENT_FULLY_DISCONNECTED OFF)
    FetchContent_MakeAvailable(${name})
endmacro()

function(fetch_add_packet)
    fetch_add_packet_macro(${ARGV})
endfunction()

if(NOT CUSTOM_COMMON_CMAKE)
    fetch_add_packet_macro(
        common_cmake GIT_REPOSITORY ${GIT_COMMON_CMAKE_REPOSITORY} GIT_TAG
        ${COMMON_CMAKE_VERSION})
    list(APPEND CMAKE_MODULE_PATH "${common_cmake_SOURCE_DIR}/cmake")
    # list(APPEND CMAKE_MODULE_PATH
    # "${FETCHCONTENT_BASE_DIR}/common_cmake-src/cmake")

    # #import include
    include(include_cmake)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
if(EXISTS ${PROJECT_SOURCE_DIR}/cmake/dependencies.cmake)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/dependencies.cmake
                   cmake/dependencies_${PROJECT_NAME}.cmake)
endif()
if(EXISTS ${PROJECT_SOURCE_DIR}/cmake/dependencies_dev.cmake)
    configure_file(${PROJECT_SOURCE_DIR}/cmake/dependencies_dev.cmake
                   cmake/dependencies_dev_${PROJECT_NAME}.cmake)
endif()
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}/cmake")
