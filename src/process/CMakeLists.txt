set(library_name "process")
add_library(${library_name} Process_posix.cpp Process_win.cpp)
add_library(${PROJECT_NAME}::${library_name} ALIAS ${library_name})

target_link_libraries(${library_name} PUBLIC ipc)

target_include_directories(
    ${library_name}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<INSTALL_INTERFACE:include/>
           $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>)

target_compile_features(${library_name} PUBLIC cxx_std_11)

# install and export configure
include(use_install_export)
