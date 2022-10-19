# Copyright (C) 2022 dobot All rights reserved.
# Author: libobo@dobot.cc. Created in 202204

include(FetchContent)
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(gtest)

