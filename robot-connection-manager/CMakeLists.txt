cmake_minimum_required(VERSION 3.16)
project(robot-connection-manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
