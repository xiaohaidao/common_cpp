set(library_name "ipc")
add_library(
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
add_library(${PROJECT_NAME}::${library_name} ALIAS ${library_name})

target_link_libraries(${library_name} PUBLIC utils)

if(WIN32)

elseif(UNIX)
    target_link_libraries(${library_name} PUBLIC rt pthread)
endif()

target_include_directories(
    ${library_name}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

target_compile_features(${library_name} PUBLIC cxx_std_11)

# install and export configure
include(use_install_export)
