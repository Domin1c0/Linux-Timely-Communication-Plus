#指定CMake最低版本要求
cmake_minimum_required(VERSION 3.10)
#设置工程名称
project(ChatServer)

# #设置C++标准为C++17
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

#设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 包含头文件目录（如果有 include 文件夹）
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#自动收集 src/ 中的所有 .cpp源文件
file(GLOB SERVER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# 查找第三方库
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVENT REQUIRED libevent)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)


#定义可执行文件 server，编译所有源码
add_executable(chat_server ${SERVER_SRC})

# 链接 MySQL 和 event 库
find_package(Threads REQUIRED)

target_link_libraries(chat_server
    mysqlclient
    event
    pthread
    jsoncpp
)