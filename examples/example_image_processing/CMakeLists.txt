file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/examples/example_image_processing/*.cpp")
include_directories(thirdparty/CImg-v.2.9.9)

add_executable(example_image_processing ${SOURCE_LIST})
target_compile_features(example_image_processing PRIVATE cxx_std_17)
target_link_libraries(example_image_processing PRIVATE lib_execution_nodes)