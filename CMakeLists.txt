cmake_minimum_required(VERSION 3.31.3)

project(Crafting-system)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_compile_options(-Wall -g -std=c99)

add_subdirectory(../Advanced-types build)
file(GLOB SOURCES "source/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE headers)

target_link_libraries(${PROJECT_NAME} PUBLIC Advanced-types)
target_link_libraries(${PROJECT_NAME} PUBLIC curl)
