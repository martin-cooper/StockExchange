cmake_minimum_required(VERSION 3.15)
project(StockExchange)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")


add_executable(StockExchange main.cpp src/Limit.cpp src/Limit.h src/Book.cpp src/Book.h src/types.h src/Engine.cpp src/Engine.h)