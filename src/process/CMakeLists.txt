set(library_name "process")
add_src_lib(${library_name} Process_posix.cpp Process_win.cpp)

target_link_libraries(${library_name} PUBLIC ipc)

target_compile_features(${library_name} PUBLIC cxx_std_11)
