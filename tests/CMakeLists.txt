# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

set(test_name "cpp_common_test")
add_executable(${test_name}
    main.cpp
    min_heap.cpp
    random.cpp
)

target_include_directories(${test_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${test_name} PRIVATE
    gtest_main
    utils
)

add_test(NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
)
