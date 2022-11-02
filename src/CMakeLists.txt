# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

add_subdirectory(ipc)
add_subdirectory(process)
add_subdirectory(utils)

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION include
)
