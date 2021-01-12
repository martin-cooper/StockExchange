cmake_minimum_required(VERSION 3.15)
project(StockExchange)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer")


add_executable(
        StockExchange
        main.cpp
        src/Limit.cpp
        src/Limit.h
        src/Book.h
        src/types.h
        src/MatchingEngine.cpp
        src/MatchingEngine.h
        src/tests/TestLimit.cpp
        src/tests/TestLimit.h
        src/tests/TestBook.cpp
        src/tests/TestBook.h src/tests/TestEngine.cpp src/tests/TestEngine.h)