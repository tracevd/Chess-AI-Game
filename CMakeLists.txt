cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

project(chess_ai)

file(GLOB_RECURSE SRC "src/*.h" "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS} ${SDL3IMAGE_INCLUDE_DIRS})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL3_LIBRARIES} SDL3_image::SDL3_image mingw32)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL3_LIBRARIES} SDL3_image::SDL3_image)
endif()
