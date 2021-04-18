cmake_minimum_required(VERSION 3.17)
project(SSR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
if (!MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual")

	set(CMAKE_CXX_FLAGS "-O3")
endif()


add_subdirectory(third-party)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)



add_executable(SSR main.cpp src/MaterialManager.cpp include/MaterialManager.h src/Shader.cpp include/Shader.h include/Logger.h include/ObjParser.h src/Mesh.cpp include/Mesh.h src/Camera.cpp include/Camera.h src/Scene.cpp include/Scene.h src/Renderer.cpp include/Renderer.h src/Timer.cpp include/Timer.h)

target_include_directories(SSR PRIVATE ${OPENGL_INCLUDE_DIR})
target_link_libraries(SSR glfw ${OPENGL_LIBRARY}  GLEW::GLEW stb_image imgui glm::glm)
set_target_properties(${PROJECT_BIN} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")