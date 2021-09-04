# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(test_execution_nodes test_execution_nodes.cpp)

# I'm using C++17 in the test
target_compile_features(test_execution_nodes PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(test_execution_nodes PRIVATE lib_execution_nodes Catch2::Catch2WithMain)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME execution_node_tests COMMAND test_execution_nodes) # Command can be a target
