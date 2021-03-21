cmake_minimum_required(VERSION 3.17)
project(SSR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS "-O3")
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(third-party)

add_executable(SSR main.cpp src/MaterialManager.cpp include/MaterialManager.h src/Shader.cpp include/Shader.h include/Logger.h include/ObjParser.h src/Mesh.cpp include/Mesh.h src/Camera.cpp include/Camera.h src/Scene.cpp include/Scene.h src/Renderer.cpp include/Renderer.h src/Timer.cpp include/Timer.h)

target_include_directories(SSR PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(SSR glfw ${OPENGL_LIBRARY} GLEW stb_image imgui)