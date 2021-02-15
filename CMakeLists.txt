cmake_minimum_required(VERSION 3.17)
project(Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(third-party)

add_executable(Project main.cpp src/Shader.cpp include/Shader.h include/Logger.h include/ObjParser.h include/Mesh.cpp include/Mesh.h include/Camera.cpp include/Camera.h)

target_include_directories(Project PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(Project glfw ${OPENGL_LIBRARY} GLEW stb_image imgui)