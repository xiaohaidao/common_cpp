# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

set(library_name "reactor")
add_library(${library_name}
    Epoll.cpp
    Select.cpp
)

target_link_libraries(${library_name} PUBLIC
)

if(WIN32)
target_link_libraries(${library_name} PUBLIC
    Ws2_32
)
elseif(UNIX)
endif()

target_include_directories(${library_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<INSTALL_INTERFACE:include/>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
)

target_compile_features(${library_name} PUBLIC
    cxx_std_11
)

# install and export configure
include(install_export)
