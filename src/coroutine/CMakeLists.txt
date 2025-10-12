set(library_name "coroutine")
add_src_lib(${library_name} coroutine.cpp)

target_compile_features(${library_name} PUBLIC cxx_std_11)
