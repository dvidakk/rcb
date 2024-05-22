cmake_minimum_required(VERSION 3.11)

project(RedditChat)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()
# if linux or macos
if(UNIX)
  find_package(OpenSSL REQUIRED)
  find_package(ZLIB REQUIRED)
endif()

set(ENABLE_CJSON_TEST Off CACHE BOOL "Disable building the tests")
set(BUILD_TESTING OFF CACHE BOOL "Disable building the tests")
set(BUILD_CURL_EXE OFF CACHE BOOL "Disable building the curl binary")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable building shared libraries") 
set(CURL_STATICLIB ON CACHE BOOL "Build libcurl as a static library")

# Add cJSON directory
add_subdirectory(vendor/cJSON)
add_subdirectory(vendor/curl)

include_directories("include")
file(GLOB_RECURSE SOURCES "src/*.c")

add_library(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} libcurl cjson)

target_include_directories(${PROJECT_NAME} PUBLIC vendor/cJSON)
target_include_directories(${PROJECT_NAME} PRIVATE vendor/curl/include)

if (WIN32)
    set_target_properties(RedditChat PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

     set(CURL_LIBRARIES "${PROJECT_BINARY_DIR}/vendor/curl/lib/libcurl_object.dir/$<CONFIG>/libcurl_object.lib")
     set(CJSON_LIBRARY  "${PROJECT_BINARY_DIR}/vendor/cJSON/$<CONFIG>/cjson.lib")
else()
    set(CURL_LIBRARIES "${PROJECT_BINARY_DIR}/vendor/curl/lib/libcurl.a")
    set(CJSON_LIBRARY  "${PROJECT_BINARY_DIR}/vendor/cJSON/libcjson.a")
endif() 

# Link the library to the executable
target_include_directories(${PROJECT_NAME} PUBLIC include)
if(UNIX)
    if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} ${CJSON_LIBRARY} OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURL_LIBRARIES} ${CJSON_LIBRARY})
endif()
