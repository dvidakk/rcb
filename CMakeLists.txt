cmake_minimum_required(VERSION 3.11)

project(RedditChat)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(ENABLE_CJSON_TEST Off CACHE BOOL "Disable building the tests")
set(BUILD_TESTING OFF CACHE BOOL "Disable building the tests")
set(BUILD_CURL_EXE OFF CACHE BOOL "Disable building the curl binary")

# Add cJSON directory
add_subdirectory(vendor/cJSON)
add_subdirectory(vendor/curl)

include_directories("include")
file(GLOB_RECURSE SOURCES "src/*.c")

add_library(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} libcurl cjson)


target_include_directories(${PROJECT_NAME} PUBLIC vendor/cJSON)
target_include_directories(${PROJECT_NAME} PUBLIC vendor/curl/include)


if (WIN32)
    set_target_properties(RedditChat PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    set(CURL_LIBRARIES "${PROJECT_BINARY_DIR}/vendor/curl/lib/libcurl_object.dir/$<CONFIG>/libcurl_object.lib")
    set(CJSON_LIBRARY  "${PROJECT_BINARY_DIR}/vendor/cJSON/$<CONFIG>/cjson.lib")
else()
    set(CURL_LIBRARIES "${PROJECT_BINARY_DIR}/vendor/curl/lib/libcurl.so")
    set(CJSON_LIBRARY  "${PROJECT_BINARY_DIR}/vendor/cJSON/libcjson.so")
endif() 

# if (WIN32)
#     add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${CMAKE_BINARY_DIR}/vendor/cJSON/$<CONFIG>/cjson.dll"
#         "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
# endif()
# copy all the dlls to the output directory for windows
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_BINARY_DIR}/libcurl.dll"
        "$<TARGET_FILE_DIR:${CMAKE_BINARY_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_BINARY_DIR}/cjson.dll"
        "$<TARGET_FILE_DIR:${CMAKE_BINARY_DIR}")
endif()
# Link the library to the executable
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} ${CJSON_LIBRARY})
