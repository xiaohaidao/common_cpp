if(NOT PROJECT_NAME)
    cmake_minimum_required(VERSION 3.14)

    project("project_test" VERSION 0.2.1)

    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        project_test_cpp
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../
        GIT_SHALLOW ON
        GIT_PROGRESS ON)
    FetchContent_MakeAvailable(project_test_cpp)
    # include(${CMAKE_CURRENT_LIST_DIR}/../cmake/get_cmake.cmake)
endif()

# include(StringVersion)
find_package(GTest REQUIRED)

set(test_name "cpp_common_test")
add_test_exe(
    ${test_name}
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
    proactor/ProactorEvent.cpp
    proactor/ProactorFiles.cpp
    proactor/ProactorIpc.cpp
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
    utils/random.cpp)

target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/src
                                                ${PROJECT_SOURCE_DIR}/include)

# if(NOT MSVC) target_link_libraries(${test_name} PRIVATE -static-libstdc++
# -static-libgcc ) endif()

target_link_libraries(
    ${test_name}
    PRIVATE GTest::gtest_main
            ipc
            proactor
            process
            reactor
            sockets
            utils
            algorithm
            coroutine)
