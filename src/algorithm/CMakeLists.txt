
set(library_name "algorithm")
add_library(${library_name} INTERFACE
)

target_link_libraries(${library_name} INTERFACE
)

if(MSVC)
target_link_libraries(${library_name} INTERFACE
)
elseif(WIN32)
target_link_libraries(${library_name} INTERFACE
    atomic
    pthread
)
elseif(UNIX)
target_link_libraries(${library_name} INTERFACE
    atomic
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
