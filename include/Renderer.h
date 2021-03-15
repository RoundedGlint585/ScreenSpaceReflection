//
// Created by roundedglint585 on 2/16/21.
//

#ifndef PROJECT_RENDERER_H
#define PROJECT_RENDERER_H

#include <GLFW/glfw3.h>
#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>
using std::size_t;
class Renderer {
public:

    Renderer() = default;

    ~Renderer();

    void init(size_t width, size_t height);

    void setScene(const Scene& scene);

    void setShader(const Shader& shader);

    void renderScene(Shader &shader);

    void renderSceneToTexture(Shader &shader);

    void renderGui();

    void runMainLoop();

    void postEffectScene(Shader &shader);

private:
    enum shaderType{
        BaseShader,
        ViewPositionShader,
        SSRShader,
        Count
    };

    struct texture{
        GLuint renderedTexture = -1;
        size_t width, height;
    };

    void renderToTexture(Shader &shader, size_t width, size_t height);
    GLFWwindow *window_m;
    size_t width_m = 512, height_m = 512;
    Scene scene_m;
    Shader shader_m;
    std::vector<Shader> shaders_m;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width_m / (float) height_m, 0.1f, 100.0f);
    float roughness, metallic, strength;
    GLuint texturePosId, textureNormalId, textureDepthId; //PreRender
    GLuint textureSceneId; // SceneRender
    GLuint postEffectSceneId; // PostEffect
    GLuint preRenderFramebufferId, sceneRenderFramebufferId;
};


#endif //PROJECT_RENDERER_H
