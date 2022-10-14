cmake_minimum_required(VERSION 3.20)

# 启动对C11标准的支持
set(CMAKE_C_STANDARD 11)

# 显式要求指明支持C标准
set(CMAKE_C_STANDARD_REQUIRED True)

# 启动对C++14标准的支持
set(CMAKE_CXX_STANDARD 14)

# 显式要求指明支持C++标准
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)
project(CLib)
add_executable(CLib ./Test/main.c)