#!/bin/sh

cmake -B build -GNinja -DBUILD_TESTING=True
cmake --build build -j
cmake --build build -j --target test
cmake --build build -j --target clang_format
cmake --build build -j --target clang_tidy

