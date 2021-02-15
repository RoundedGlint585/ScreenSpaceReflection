#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "include/Shader.h"
#include "include/Mesh.h"
#include "include/ObjParser.h"
#include "include/Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
    GLFWwindow *window_;
    uint32_t width_ = 512;
    uint32_t height_ = 512;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window_ = glfwCreateWindow(width_, height_, "Object", nullptr, nullptr);
    glfwMakeContextCurrent(window_);
    glViewport(0, 0, width_, height_);
    glewInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    Shader shader = Shader::fromFile("shaders/baseVertex.glsl", "shaders/baseFragment.glsl");
    shader.use();
    auto [vertices, indices] = objParser::parseFile("objects/cube.obj");
    Mesh mesh = Mesh(indices, vertices);
    Camera camera({2,1, 5.}, {0, 0, 0}, {0.f, 1.f, 0});
    glm::mat4 view_ = camera.getViewMatrix();
    glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f),glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width_ / (float) height_, 0.1f, 100.0f);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load("textures/wall.jpg", &width, &height, &nrChannels, 0);
    std::cout << width << " " << height << std::endl;
    mesh.setTexture(data, width, height);
    //stbi_image_free(data);
    while (!glfwWindowShouldClose(window_)) {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view_ );
        shader.setMat4("projection", projection);
        shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
        shader.setVec3("lightPos", {0.f, 0.f, 3.f});
        mesh.draw(shader);

        // GUI
        ImGui::Begin("Dissolve window");
        float test;
        ImGui::SliderFloat("Threshold Count", &test, 0.f, 1.f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
    return 0;
}
