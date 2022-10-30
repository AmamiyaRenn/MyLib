cmake_minimum_required(VERSION 3.20)

# 启动对C17标准的支持
set(CMAKE_C_STANDARD 17)
# 显式要求指明支持C标准
set(CMAKE_C_STANDARD_REQUIRED True)
# 启动对C++17标准的支持
set(CMAKE_CXX_STANDARD 17)
# 显式要求指明支持C++标准
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)

aux_source_directory(OS OS_SRC)
project(CLib)
add_executable(CLib ./Test/main.c ${OS_SRC})
target_link_libraries(CLib -lpthread)