cmake_minimum_required(VERSION 3.10)

project(ChatServer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 自动收集 src/ 中的所有 .cpp 源文件
file(GLOB SERVER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# 定义可执行文件
add_executable(chat_server ${SERVER_SRC})

# 查找第三方库
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# 链接库
target_link_libraries(chat_server
    mysqlclient
    event
    pthread
    jsoncpp
)
