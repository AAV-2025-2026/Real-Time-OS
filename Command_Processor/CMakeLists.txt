cmake_minimum_required(VERSION 3.10)
project(CommandProcessor)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

file(GLOB SOURCES "src/*.cpp")

add_executable(main_example ${SOURCES})