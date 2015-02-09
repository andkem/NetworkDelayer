cmake_minimum_required(VERSION 2.6)
project(networkdelayer)

add_executable(networkdelayer main.cpp)

install(TARGETS networkdelayer RUNTIME DESTINATION bin)
