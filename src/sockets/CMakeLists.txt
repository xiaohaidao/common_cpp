
set(library_name "sockets")
add_library(${library_name}
    SocketAddr.cpp
    sockets.cpp
    TcpListener.cpp
    TcpStream.cpp
    UdpSocket.cpp
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
