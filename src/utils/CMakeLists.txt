set(library_name "utils")
add_src_lib(${library_name} log.cpp random.cpp)

target_compile_features(${library_name} PRIVATE cxx_std_11)
