cmake_minimum_required(VERSION 3.16)
project(micro_acc_system)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${SOURCE_DIR})

file(GLOB SOURCE_HPP "${SOURCE_DIR}/*.hpp")
file(GLOB SOURCE_CPP "${SOURCE_DIR}/*.cpp")

file(GLOB MAIN_CPP "test/Micro-test.cpp")

set(SYSTEMC_INCLUDE_DIR "/usr/local/include")
set(SYSTEMC_LIBRARY_DIR "/usr/local/lib64")
include_directories(${SYSTEMC_INCLUDE_DIR})
link_directories(${SYSTEMC_LIBRARY_DIR})

add_executable(micro_acc_system ${SOURCE_HPP} ${SOURCE_CPP} ${MAIN_CPP})
target_link_libraries(micro_acc_system systemc)