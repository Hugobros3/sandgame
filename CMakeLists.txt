cmake_minimum_required(VERSION 3.27)
project(sangame)

set(CMAKE_C_STANDARD 11)

add_subdirectory(cunk)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(sandgame main.c sandgame.c sandgame_host.c)
target_link_libraries(sandgame PRIVATE graphics)

find_package(OpenMP)
if(OpenMP_C_FOUND)
    target_link_libraries(sandgame PUBLIC OpenMP::OpenMP_C)
endif()

set_property(TARGET sandgame PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)