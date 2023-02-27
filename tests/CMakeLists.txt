
set(test_name "cpp_common_test")
add_executable(${test_name}
    main.cpp
    min_heap.cpp
    ipc/MsgQueue.cpp
    ipc/Pipe.cpp
    ipc/Semaphores.cpp
    ipc/SharedMemory.cpp
    proactor/Proactor.cpp
    process/Process.cpp
    reactor/Reactor.cpp
    sockets/SocketAddr.cpp
    sockets/Tcp.cpp
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
)

add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
)
