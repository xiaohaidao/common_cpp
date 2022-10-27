# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

set(library_name "ipc")
add_library(${library_name}
    # SharedMemory_sysv.cpp
    SharedMemory_posix.cpp
    SharedMemory.cpp
)

target_link_libraries(${library_name} PUBLIC
    rt
    utils
)

target_include_directories(${library_name} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/include/
)

#set_target_properties((${library_name}
    #PROPERTIES
    #PUBLIC_HEADER $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
#)

target_compile_features(${library_name} PUBLIC
    # cxx_std_11
)

install(TARGETS ${library_name}
)
