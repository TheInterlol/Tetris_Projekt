cmake_minimum_required(VERSION 3.10)
project(Tetris C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(Tetris main.c)

target_link_libraries(Tetris PRIVATE 
    SDL2::SDL2
    SDL2_image::SDL2_image
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)