# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202210

add_custom_target(clang_format
    COMMAND clang-format -style=file -i `find src include example tests
        -iname \"*.cpp\" -o -iname \"*.h\" -o -iname \"*.c\"`
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Format code with clang-format"
)
