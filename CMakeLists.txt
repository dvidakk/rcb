cmake_minimum_required(VERSION 3.11)

# Set the project name
project(RedditChat)

#set(CMAKE_C_STANDARD 17)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Add cJSON directory
add_subdirectory(vendor/cJSON)
add_subdirectory(vendor/curl)

# source files in a src directory
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
add_dependencies(${CMAKE_PROJECT_NAME} libcurl cjson)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE vendor/cJSON)

if (WIN32)
    set(CURL_LIBRARIES "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl_object.dir/$<CONFIG>/libcurl_object.lib")
    set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/$<CONFIG>/cjson.lib")
else()
    set(CURL_LIBRARIES "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl.so")
    set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/libcjson.so")
endif()

if (WIN32)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/vendor/cJSON/$<CONFIG>/cjson.dll"
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>")
endif()

# Link the library to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} ${CJSON_LIBRARY})
