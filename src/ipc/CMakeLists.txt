set(library_name "ipc")
add_src_lib(
    ${library_name}
    mm_socket.cpp
    MsgQueue_null.cpp
    MsgQueue_posix.cpp
    MsgQueue.cpp
    Pipe_posix.cpp
    Pipe_win.cpp
    PipeListener_posix.cpp
    PipeListener_win.cpp
    PipeStream_posix.cpp
    PipeStream_win.cpp
    Semaphores_posix.cpp
    Semaphores_win.cpp
    SharedMemory_posix.cpp
    SharedMemory_win.cpp
    SharedMemory.cpp)

target_link_libraries(${library_name} PUBLIC utils)

if(WIN32)

elseif(UNIX)
    target_link_libraries(${library_name} PUBLIC rt pthread)
endif()

target_compile_features(${library_name} PUBLIC cxx_std_11)
