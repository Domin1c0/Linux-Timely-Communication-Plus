#指定CMake最低版本要求
cmake_minimum_required(VERSION 3.10)
#设置工程名称
project(ChatServer)
#设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
#设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#包含头文件目录
include_directories(include)
#自动收集 src/ 中的所有 .cpp源文件
file(GLOB SOURCES "src/*.cpp")
#定义可执行文件 server，编译所有源码
add_executable(server ${SOURCES})