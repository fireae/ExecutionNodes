file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/examples/example_arithmetic/*.cpp")

add_executable(example_arithmetic ${SOURCE_LIST})
target_compile_features(example_arithmetic PRIVATE cxx_std_17)
target_link_libraries(example_arithmetic PRIVATE lib_execution_nodes)