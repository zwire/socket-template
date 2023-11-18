cmake_minimum_required(VERSION 3.21)

project(socket-template)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install)

enable_testing()

add_subdirectory(cpp)
