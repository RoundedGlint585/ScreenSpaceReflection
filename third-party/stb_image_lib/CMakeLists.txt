cmake_minimum_required(VERSION 3.1)

project(stb_image)


set(HEADERS
        libstbimages/stb_image.h)

set(CMAKE_CXX_STANDARD 17)

set(LIBRARIES)


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE stb_image/)
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})