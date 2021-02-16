//
// Created by roundedglint585 on 2/16/21.
//

#include <GL/glew.h>
#include "../include/Renderer.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Renderer::init(size_t width, size_t height) {
    width_m = width;
    height_m = height;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window_m = glfwCreateWindow(width_m, height_m, "Object", nullptr, nullptr);
    glfwMakeContextCurrent(window_m);
    glViewport(0, 0, width_m, height_m);
    glewInit();


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window_m, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    projection = glm::perspective(glm::radians(45.0f), (float) width_m / (float) height_m, 0.1f, 100.0f);
}

void Renderer::setScene(const Scene &scene) {
    scene_m = scene;
}

void Renderer::render() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    shader_m.use();
    shader_m.setMat4("projection", projection);
    scene_m.render(shader_m);

    ImGui::Begin("Dissolve window");

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_m);
    glfwPollEvents();

}

void Renderer::setShader(const Shader &shader) {
    shader_m = shader;

}

void Renderer::runMainLoop() {
    while (!glfwWindowShouldClose(window_m)) {
        render();
    }
}
