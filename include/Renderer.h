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
#include <array>
#include "Timer.h"
using std::size_t;
class Renderer {
public:

    Renderer() = default;

    ~Renderer();

    void init(size_t width, size_t height);

    void setScene(const Scene& scene);

    void renderScene(Shader &shader, const glm::vec3 &backgroundColor = {0.2f, 0.2f, 0.2f});

    void renderSceneToTexture(Shader &shader);

    void renderGui();

    void runMainLoop();

    void postEffectScene(Shader &shader);

private:
    enum shaderType{
        PreRender_Shader,
        Base_Shader,
        SSR_Shader,
        Count
    };

    void renderToTexture(Shader &shader, size_t width, size_t height);
    void createEmptyTexture(GLuint &textureId, size_t width, size_t height, GLuint type);
    GLFWwindow *window_m;
    size_t width_m = 512, height_m = 512;
    Scene scene_m;
    std::vector<Shader> shaders_m;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) width_m / (float) height_m, 0.1f, 100.0f);
    GLuint textureNormalId, textureDepthId, textureMetallicId; //PreRender
    GLuint textureSceneId; // SceneRender
    GLuint preRenderFramebufferId, sceneRenderFramebufferId;
    GLuint postProcessVAO;
    //ssr shader variables
    bool debugDraw = false;
    bool isSSREnabled = true;
    float distanceBias = 0.03, rayStep = 0.1;
    int ssrIterationCount = 100;
    // sampling settings
    bool isAdaptiveStepEnabled = true;
    bool isBinarySearchEnabled = true;
    bool isExponentialStepEnabled = false;
    bool isSamplingEnabled = false;
    int sampleCount = 4;
    float samplingCoefficient = 0.01;
    //texture checker
    std::array<int, 3> textureType;
    int selectedTexture;

    timer::Timer timer;
    float renderingTime;
    float timeAccumulated;
    int accumulationWindow = 100;
    int accumulatedFrames = 0;
    void loadShaders();

    void initPreRenderFramebuffer();

    void initSceneRenderFramebuffer();

    void initGUI() const;
};


#endif //PROJECT_RENDERER_H
