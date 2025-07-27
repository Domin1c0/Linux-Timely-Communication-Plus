cmake_minimum_required(VERSION 3.10)

project(ChatClient)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 源码文件
file(GLOB CLIENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(chat_client ${CLIENT_SRC})

# 链接库
target_link_libraries(chat_client
    pthread
    jsoncpp
)
