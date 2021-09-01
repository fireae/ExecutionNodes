# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/include/*/*.h*")
#set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/modern/lib.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/src/*.cpp")


# Make an automatic library - will be static or dynamic based on user setting
add_library(lib_execution_nodes execution_nodes.cpp ${HEADER_LIST})

# Add source files
target_sources(lib_execution_nodes PRIVATE ${SOURCE_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(lib_execution_nodes PUBLIC ../include)
target_include_directories(lib_execution_nodes PUBLIC ../third_party/nlohmann/v3.10.2/single_include)
target_include_directories(lib_execution_nodes PRIVATE ../third_party/spdlog/v1.9.2/include)


# All users of this library will need at least C++17
target_compile_features(lib_execution_nodes PUBLIC cxx_std_17)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})