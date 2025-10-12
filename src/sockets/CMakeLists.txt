set(library_name "sockets")
add_src_lib(
    ${library_name}
    IcmpSocket.cpp
    SocketAddr.cpp
    sockets.cpp
    TcpListener.cpp
    TcpStream.cpp
    UdpSocket.cpp)

if(WIN32)
    target_link_libraries(${library_name} PUBLIC Ws2_32 IPHLPAPI)
elseif(UNIX)

endif()

target_compile_features(${library_name} PUBLIC cxx_std_11)
