cmake_minimum_required(VERSION 3.16.3)
project(MasimoRoot)

set(CMAKE_CXX_STANDARD 17)

add_executable(MasimoRoot main.cpp Socket.cpp Socket.h SerialPort.cpp SerialPort.h)