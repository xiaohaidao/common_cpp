
set(library_name "coroutine")
add_library(${library_name}
    coroutine.cpp
)

target_link_libraries(${library_name} PUBLIC
)

target_include_directories(${library_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
)

target_compile_features(${library_name} PUBLIC
    cxx_std_11
)

# install and export configure
include(install_export)
