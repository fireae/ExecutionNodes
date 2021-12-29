file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/examples/example_logic_gates/*.cpp")

add_executable(example_logic_gates ${SOURCE_LIST})
target_compile_features(example_logic_gates PRIVATE cxx_std_17)
target_link_libraries(example_logic_gates PRIVATE lib_execution_nodes)

if (UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif (UNIX)
