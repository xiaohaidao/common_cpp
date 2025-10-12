set(library_name "reactor")
add_src_lib(${library_name} Epoll.cpp Select.cpp)

if(WIN32)
    target_link_libraries(${library_name} PUBLIC Ws2_32)
elseif(UNIX)

endif()

target_compile_features(${library_name} PUBLIC cxx_std_11)
