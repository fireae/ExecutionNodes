cmake_minimum_required(VERSION 3.14)

project(
  ExecutionNodes
  VERSION 0.1
  DESCRIPTION "ExecutionNodes"
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  set(CMAKE_CXX_EXTENSIONS OFF)
  #set_property(TARGET ExecutionNodes PROPERTY CXX_STANDARD 17)

  # Support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14; use our shim
#if(${CMAKE_VERSION} VERSION_LESS 3.14)
#  include(cmake/add_FetchContent_MakeAvailable.cmake)
#endif()

# Accumulator library
# This is header only, so could be replaced with git submodules or FetchContent
#find_package(Boost REQUIRED)
# Adds Boost::boost

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(examples)

# Testing only available if this is the main app
# Emergency override EXECUTION_NODES_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR EXECUTION_NODES_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
