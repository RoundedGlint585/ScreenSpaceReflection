//
// Created by roundedglint585 on 2/16/21.
//

#include <GL/glew.h>
#include "../include/Renderer.h"
#include "../include/Logger.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Renderer::init(size_t width, size_t height) {
    width_m = width;
    height_m = height;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window_m = glfwCreateWindow(width_m, height_m, "Object", nullptr, nullptr);
    glfwMakeContextCurrent(window_m);
    glViewport(0, 0, width_m, height_m);
    glewInit();

    initGUI();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    projection = glm::perspective(glm::radians(45.0f), (float) width_m / (float) height_m, 0.1f, 100.0f);

    loadShaders();

    initPreRenderFramebuffer();

    initSceneRenderFramebuffer();

    glGenVertexArrays(1, &postProcessVAO);


}

void Renderer::initGUI() const {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window_m, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void Renderer::initSceneRenderFramebuffer() {
    sceneRenderFramebufferId = 0;
    glGenFramebuffers(1, &sceneRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneRenderFramebufferId);
    createEmptyTexture(textureSceneId, width_m, height_m, GL_RGBA16);

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width_m, height_m);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureSceneId, 0);
    GLenum SceneDrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, SceneDrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logger::ERROR("Frame buffer not created");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initPreRenderFramebuffer() {
    glGenFramebuffers(1, &preRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, preRenderFramebufferId);

    createEmptyTexture(textureNormalId, width_m, height_m, GL_RGBA16);
    createEmptyTexture(textureDepthId, width_m, height_m, GL_RGBA16);
    createEmptyTexture(textureMetallicId, width_m, height_m, GL_RGBA16F);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNormalId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureDepthId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textureMetallicId, 0);

    //depth buffer attachment
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_m, height_m);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logger::ERROR("Frame buffer not created");
    }
}

void Renderer::loadShaders() {
    logger::INFO("Compiling view position shader");
    shaders_m.push_back(Shader::fromFile("shaders/preRender.vertex.glsl", "shaders/preRender.fragment.glsl"));

    logger::INFO("Compiling base shader");
    shaders_m.push_back(Shader::fromFile("shaders/baseVertex.glsl", "shaders/baseFragment.glsl"));

    logger::INFO("Compiling SSR shader");
    shaders_m.push_back(Shader::fromFile("shaders/SSR.vertex.glsl", "shaders/SSRFragment.glsl"));
}

void Renderer::setScene(const Scene &scene) {
    scene_m = scene;
}

void Renderer::renderScene(Shader &shader, const glm::vec3 &backgroundColor) {
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec2("viewPortSize", {width_m, height_m});
    shader.setMat4("invProj", glm::inverse(projection));
    scene_m.render(shader);
}

void Renderer::runMainLoop() {
    while (!glfwWindowShouldClose(window_m)) {
        renderToTexture(shaders_m[PreRender_Shader], width_m, height_m);
        renderSceneToTexture(shaders_m[Base_Shader]);
        postEffectScene(shaders_m[SSR_Shader]);
        renderGui();
        glfwSwapBuffers(window_m);
        glfwPollEvents();
    }
}

Renderer::~Renderer() {
    glfwDestroyWindow(window_m);
}

void Renderer::renderToTexture(Shader &shader, size_t width, size_t height) {
    shader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, preRenderFramebufferId);
    glViewport(0, 0, width, height);
    renderScene(shader, {0.f, 0.f, 0.f});
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderSceneToTexture(Shader &shader) {
    shader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, sceneRenderFramebufferId);
    glViewport(0, 0, width_m, height_m);
    renderScene(shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::postEffectScene(Shader &shader) {
    glViewport(0, 0, width_m, height_m);
    shader.use();
    glBindVertexArray(postProcessVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureNormalId);
    shader.setInt("tNorm", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSceneId);
    shader.setInt("tFrame", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureMetallicId);
    shader.setInt("tMetallic", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureDepthId);
    shader.setInt("tDepth", 3);

    shader.setMat4("invView", glm::inverse(scene_m.getCamera().getViewMatrix()));
    shader.setMat4("view", scene_m.getCamera().getViewMatrix());
    shader.setMat4("proj", projection);
    shader.setMat4("invProj", glm::inverse(projection));
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Renderer::renderGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Dissolve window");
    ImGui::SliderFloat("spec", &comparingBias, 0.0001, 1.f);//temp sol
    ImGui::SliderFloat("rayStep", &rayStep, 0.01, 0.5f);
    ImGui::SliderFloat("strength", &strength, 0.0001, 1.f);
    ImGui::SliderInt("iterationCount", &ssrIterationCount, 10, 100);
    ImGui::Checkbox("Enable SSR", &isSSREnabled);
    shaders_m[2].use();
    shaders_m[2].setFloat("spec", comparingBias);
    shaders_m[2].setFloat("rayStep", rayStep);
    shaders_m[2].setFloat("strength", strength);
    shaders_m[2].setBool("enableSSR", isSSREnabled);
    shaders_m[2].setInt("iterationCount", ssrIterationCount);
    ImGui::End();
    ImGui::Begin("Texture check");
    ImGui::Image((void *) (intptr_t) textureSceneId, ImVec2(width_m, height_m), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::createEmptyTexture(GLuint &textureId, size_t width, size_t height, GLuint type = GL_RGBA16) {
    glGenTextures(1, &textureId);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}



