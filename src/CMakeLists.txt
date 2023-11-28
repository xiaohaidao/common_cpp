
add_subdirectory(algorithm)
add_subdirectory(coroutine)
add_subdirectory(ipc)
add_subdirectory(proactor)
add_subdirectory(process)
add_subdirectory(reactor)
add_subdirectory(sockets)
add_subdirectory(utils)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include
)

set(library_name proactor reactor process utils)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
    add_executable(${PROJECT_NAME} main.cpp)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${library_name}
    )
endif()
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/bin)
    file(GLOB_RECURSE bin_files "bin/*.cpp")
    foreach(f ${bin_files})
        get_filename_component(name ${f} NAME_WE)
        add_executable(${name} ${f})

        target_link_libraries(${name} PRIVATE
            ${library_name}
        )
    endforeach()
endif()
