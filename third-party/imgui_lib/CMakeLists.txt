cmake_minimum_required(VERSION 3.1)

project(imgui)


set(HEADERS
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_impl_glfw.h
        imgui/imgui_impl_opengl3.h
        )

set(SOURCES
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp)

set(CMAKE_CXX_STANDARD 17)

set(LIBRARIES)


add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
