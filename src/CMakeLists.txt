# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/include/public/*/*.h*")
file(GLOB PRIVATE_HEADER_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/include/private/execution_nodes/internal/*.h*")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${ExecutionNodes_SOURCE_DIR}/src/*.cpp")


# Make an automatic library - will be static 
add_library(lib_execution_nodes STATIC execution_nodes.cpp ${PUBLIC_HEADER_LIST} ${PRIVATE_HEADER_LIST})

# Add source files
target_sources(lib_execution_nodes PRIVATE ${SOURCE_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(lib_execution_nodes PUBLIC ../include/public)
target_include_directories(lib_execution_nodes PRIVATE ../include/private)

if(USE_EXTERNAL_NLOHMANN_JSON)
  find_package(nlohmann_json 3.10.2 REQUIRED)
else()
  FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.10.2)

  FetchContent_GetProperties(json)
  if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)

  endif()
endif()
target_link_libraries(lib_execution_nodes PUBLIC nlohmann_json::nlohmann_json)


# All users of this library will need at least C++17
target_compile_features(lib_execution_nodes PUBLIC cxx_std_17)
if (WIN32)
  if (EXECUTION_NODES_STATIC)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
  endif(EXECUTION_NODES_STATIC)
endif (WIN32)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include/public"
  PREFIX "Public Header Files"
  FILES ${PUBLIC_HEADER_LIST})

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include/private"
  PREFIX "Private Header Files"
  FILES ${PRIVATE_HEADER_LIST})