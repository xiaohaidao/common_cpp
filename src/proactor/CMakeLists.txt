
set(library_name "proactor")
add_library(${library_name}
    Proactor_posix.cpp
    Proactor_win.cpp
    operation/FilesOp.cpp
    operation/TcpListenerOp.cpp
    operation/TcpStreamOp.cpp
    operation/UdpSocketOp.cpp
    operation/detail/AcceptOp_posix.cpp
    operation/detail/AcceptOp_win.cpp
    operation/detail/ConnectOp_posix.cpp
    operation/detail/ConnectOp_win.cpp
    operation/detail/ReadOp_posix.cpp
    operation/detail/ReadOp_win.cpp
    operation/detail/RecvFromOp_posix.cpp
    operation/detail/RecvFromOp_win.cpp
    operation/detail/RecvOp_posix.cpp
    operation/detail/RecvOp_win.cpp
    operation/detail/SendOp_posix.cpp
    operation/detail/SendOp_win.cpp
    operation/detail/SendToOp_posix.cpp
    operation/detail/SendToOp_win.cpp
    operation/detail/WriteOp_posix.cpp
    operation/detail/WriteOp_win.cpp
)

target_link_libraries(${library_name} PUBLIC
  sockets
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
