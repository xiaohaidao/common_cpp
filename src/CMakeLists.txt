
add_subdirectory(ipc)
add_subdirectory(proactor)
add_subdirectory(process)
add_subdirectory(reactor)
add_subdirectory(sockets)
add_subdirectory(stun)
add_subdirectory(utils)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include
)
