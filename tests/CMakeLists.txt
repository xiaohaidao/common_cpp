
include(StringVersion)

set(test_name "cpp_common_test")
add_executable(${test_name}
    # main.cpp
    algorithm/interrupt_thread_test1.cpp
    algorithm/lock_free_queue_test1.cpp
    algorithm/lock_free_stack_test1.cpp
    algorithm/min_heap.cpp
    algorithm/thread_pool_test1.cpp
    coroutine/test_coroutine.cpp
    ipc/mm_socket.cpp
    ipc/MsgQueue.cpp
    ipc/Pipe.cpp
    ipc/Semaphores.cpp
    ipc/SharedMemory.cpp
    proactor/Proactor.cpp
    proactor/ProactorCo.cpp
    proactor/ProactorFiles.cpp
    proactor/ProactorTimer.cpp
    proactor/ProactorUdp.cpp
    process/Process.cpp
    reactor/Reactor.cpp
    reactor/ReactorUdp.cpp
    sockets/Icmp.cpp
    sockets/SocketAddr.cpp
    sockets/Tcp.cpp
    sockets/Udp.cpp
    utils/log.cpp
    utils/random.cpp
)

target_include_directories(${test_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    set(VLD_DIR "C:/Program Files (x86)/Visual Leak Detector")
    if (EXISTS "${VLD_DIR}/include/vld.h")
        target_compile_definitions(${test_name} PRIVATE HAS_VLD_H)
    endif()
    target_include_directories(${test_name} PRIVATE
        "${VLD_DIR}/include"
    )
    target_link_directories(${test_name} PRIVATE
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            "${VLD_DIR}/lib/Win64"
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            "${VLD_DIR}/lib/Win32"
        endif()
    )
endif()
# if(NOT MSVC)
# target_link_libraries(${test_name} PRIVATE
#     -static-libstdc++
#     -static-libgcc
# )
# endif()

target_link_libraries(${test_name} PRIVATE
    GTest::gtest_main
    ipc
    proactor
    process
    reactor
    sockets
    utils
    algorithm
    coroutine
)

add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
)
