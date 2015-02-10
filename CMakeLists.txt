cmake_minimum_required(VERSION 2.6)
project(networkdelayer)

find_package(Boost COMPONENTS system program_options thread REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11")

set(SRC_FILES main.cpp server.cpp connection.cpp)

add_executable(networkdelayer ${SRC_FILES})

target_link_libraries(networkdelayer ${Boost_LIBRARIES} pthread)

install(TARGETS networkdelayer RUNTIME DESTINATION bin)
