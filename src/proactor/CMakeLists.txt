# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

set(library_name "proactor")
add_library(${library_name}
    Proactor_win.cpp
    operation/TcpListenerOp_win.cpp
    operation/TcpStreamOp_win.cpp
    operation/detail/AcceptOp_win.cpp
    operation/detail/ConnectOp_win.cpp
    operation/detail/RecvFromOp_win.cpp
    operation/detail/RecvOp_win.cpp
    operation/detail/SendOp_win.cpp
    operation/detail/SendToOp_win.cpp
)

target_link_libraries(${library_name} PUBLIC
  sockets
  utils
)

if(WIN32)
target_link_libraries(${library_name} PUBLIC
    mswsock
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
