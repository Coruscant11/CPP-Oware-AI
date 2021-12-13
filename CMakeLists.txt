cmake_minimum_required(VERSION 3.20)
project(AwaleCustom)

set(CMAKE_CXX_STANDARD 23)

add_executable(AwaleCustom
        main.cpp
        Board.cpp
        Board.h
        AI.cpp
        AI.h
        Engine.cpp
        Engine.h
        hash.h)
