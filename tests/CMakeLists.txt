
set(test_name "cpp_common_test")
add_executable(${test_name}
    main.cpp
    algorithm/interrupt_thread_test1.cpp
    algorithm/lock_free_queue_test1.cpp
    algorithm/lock_free_stack_test1.cpp
    algorithm/min_heap.cpp
    algorithm/thread_pool_test1.cpp
    ipc/MsgQueue.cpp
    ipc/Pipe.cpp
    ipc/Semaphores.cpp
    ipc/SharedMemory.cpp
    proactor/Proactor.cpp
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
    stun/stun.cpp
    utils/log.cpp
    utils/random.cpp
)

target_include_directories(${test_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${test_name} PRIVATE
    gtest_main
    ipc
    proactor
    process
    reactor
    sockets
    stun
    utils
    algorithm
)

add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
)
