


if(EXECUTION_NODES_USE_EXTERNAL_JSON)
    find_package(nlohmann_json 3.10.2 REQUIRED)
else()
    #set(JSON_BuildTests OFF CACHE INTERNAL "")
    #add_subdirectory(nlohmann_json)

    FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_TAG v3.10.2)

    FetchContent_GetProperties(json)
    if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

endif()


