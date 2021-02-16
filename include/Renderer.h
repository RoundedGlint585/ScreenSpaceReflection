//
// Created by roundedglint585 on 2/16/21.
//

#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H

#include <GLFW/glfw3.h>
#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Renderer {
public:

    Renderer() = default;

    void init(size_t width, size_t height);

    void setScene(const Scene& scene);

    void setShader(const Shader& shader);

    void render();

    void runMainLoop();

private:
    GLFWwindow *window_m;
    size_t width_m = 512, height_m = 512;
    Scene scene_m;
    Shader shader_m;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width_m / (float) height_m, 0.1f, 100.0f);
};


#endif //PROJECT_RENDERER_H
