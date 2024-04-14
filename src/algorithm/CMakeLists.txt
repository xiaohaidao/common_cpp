
set(library_name "algorithm")
add_library(${library_name} INTERFACE
)
add_library(${PROJECT_NAME}::${library_name} ALIAS ${library_name})

target_link_libraries(${library_name} INTERFACE
)

if(MSVC)
target_link_libraries(${library_name} INTERFACE
)
else()
target_link_libraries(${library_name} INTERFACE
    atomic.a
    pthread
)
endif()

target_include_directories(${library_name}
    INTERFACE $<INSTALL_INTERFACE:include/algorithm/>
    INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/algorithm/>
)

target_compile_features(${library_name} INTERFACE
    cxx_std_11
)

# install and export configure
include(install_export)
