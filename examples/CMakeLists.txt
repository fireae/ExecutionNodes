
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/examples/*.cpp")

add_executable(example ${SOURCE_LIST})
target_compile_features(example PRIVATE cxx_std_17)
target_link_libraries(example PRIVATE lib_execution_nodes)