cmake_minimum_required(VERSION 3.10)

# 客户端模块
project(ChatClient)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 源码文件
file(GLOB CLIENT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(chat_client ${CLIENT_SRC})

target_link_libraries(chat_client
    pthread
    jsoncpp
)
