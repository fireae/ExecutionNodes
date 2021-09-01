add_executable(example example.cpp)
target_compile_features(example PRIVATE cxx_std_17)
target_include_directories(example PRIVATE ../third_party/nlohmann/v3.10.2/single_include)
target_link_libraries(example PRIVATE lib_execution_nodes)