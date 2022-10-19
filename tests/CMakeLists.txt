# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

set(test_name "your_name")
add_executable(${test_name}
    main.cpp
)

target_include_directories(${test_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${test_name} PRIVATE
    gtest_main
)

add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
)
