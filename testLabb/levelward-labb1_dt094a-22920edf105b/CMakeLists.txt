cmake_minimum_required(VERSION 4.0)
project(labb1_dt094a)

set(CMAKE_CXX_STANDARD 20)

add_executable(labb1_dt094a main.cpp
        CoapClient.cpp
        CoapClient.h
        CoapMessage.cpp
        CoapMessage.h)

# Link Windows Socket library
target_link_libraries(labb1_dt094a ws2_32)