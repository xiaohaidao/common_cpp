
set(library_name "ipc")
add_library(${library_name}
    MsgQueue_null.cpp
    MsgQueue_posix.cpp
    MsgQueue.cpp
    Semaphores_posix.cpp
    Semaphores_win.cpp
    Semaphores.cpp
    SharedMemory_posix.cpp
    SharedMemory_win.cpp
    SharedMemory.cpp
)

target_link_libraries(${library_name} PUBLIC
)

if(WIN32)
elseif(UNIX)
target_link_libraries(${library_name} PUBLIC
    rt
    pthread
)
endif()

target_include_directories(${library_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
)

target_compile_features(${library_name} PUBLIC
    cxx_std_11
)

# install and export configure
include(install_export)
