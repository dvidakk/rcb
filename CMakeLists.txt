cmake_minimum_required(VERSION 3.11)

# Set the project name
project(RedditChat)


# Set the C standard to the newest version
#set(CMAKE_C_STANDARD 11)
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

add_executable(RedditChat ${SOURCES})
add_dependencies(RedditChat libcurl cjson)

# Set the option to not build cJSON tests
set(CJSON_BuildTests OFF CACHE INTERNAL "")
set(CJSON_BuildExample OFF CACHE INTERNAL "")

# Set the option to not build libcurl tests
set(BUILD_TESTING OFF CACHE INTERNAL "")

# # Set the option to not build libcurl examples
# set(BUILD_CURL_EXE OFF CACHE INTERNAL "")

target_include_directories(RedditChat PRIVATE vendor/cJSON)

if (WIN32)
    set(CURL_LIBRARIES "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl_object.dir/Debug/libcurl_object.lib")
    set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/Debug/cjson.lib")
else()
    set(CURL_LIBRARIES "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl.so")
    set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/libcjson.so")
endif()
# # # Set the path to the static cJSON and libcurl libraries
# set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/libcjson.so")
# # set(CURL_LIBRARY "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl.so")

if (WIN32)
    add_custom_command(TARGET RedditChat POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/vendor/cJSON/Debug/cjson.dll"
        "$<TARGET_FILE_DIR:RedditChat>")

endif()

# Link the library to the executable
target_link_libraries(RedditChat PRIVATE ${CURL_LIBRARIES} ${CJSON_LIBRARY})
