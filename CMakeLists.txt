cmake_minimum_required(VERSION 3.10)
project(ChatSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 全局编译选项
add_compile_options(-Wall -O2)

# 添加子目录
add_subdirectory(ChatServer)
add_subdirectory(ChatClient)
