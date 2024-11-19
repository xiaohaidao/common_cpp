add_subdirectory(algorithm)
add_subdirectory(coroutine)
add_subdirectory(ipc)
add_subdirectory(proactor)
add_subdirectory(process)
add_subdirectory(reactor)
add_subdirectory(sockets)
add_subdirectory(utils)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION include)

set(library_name proactor reactor process utils)

add_src_exe(${CMAKE_CURRENT_LIST_DIR} ${library_name})
