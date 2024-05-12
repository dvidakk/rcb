cmake_minimum_required(VERSION 3.10)

# Set the project name
project(RedditChat)

#find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
#find_package(CJSON REQUIRED)

# Set the C standard to the newest version
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic ")
# source files in a src directory
file(GLOB_RECURSE SOURCES "src/*.c")
# Add the executable
# add_library(RedditChat STATIC ${SOURCES})


# Add cJSON directory
add_subdirectory(vendor/cJSON)
add_subdirectory(vendor/curl)

add_executable(RedditChat ${SOURCES})
add_dependencies(RedditChat libcurl cjson)

target_compile_options(RedditChat PRIVATE -Wall -Wextra -pedantic -static)

set(BUILD_SHARED_LIBS OFF)

# Set the option to not build cJSON tests
set(CJSON_BuildTests OFF CACHE INTERNAL "")
set(CJSON_BuildExample OFF CACHE INTERNAL "")

# Set the option to not build libcurl tests
set(BUILD_TESTING OFF CACHE INTERNAL "")

# Set the option to not build libcurl examples
set(BUILD_CURL_EXE OFF CACHE INTERNAL "")

target_include_directories(RedditChat PRIVATE vendor/cJSON)

# # Set the path to the static cJSON and libcurl libraries
set(CJSON_LIBRARY "${CMAKE_BINARY_DIR}/vendor/cJSON/libcjson.so")
set(CURL_LIBRARY "${CMAKE_BINARY_DIR}/vendor/curl/lib/libcurl.so")

# Link the library to the executable
target_link_libraries(RedditChat PRIVATE ${CURL_LIBRARY} ${CJSON_LIBRARY} Threads::Threads)
