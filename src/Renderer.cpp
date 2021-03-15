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

    logger::INFO("Compiling base shader");
    shaders_m.push_back(Shader::fromFile("shaders/baseVertex.glsl", "shaders/baseFragment.glsl"));
    logger::INFO("Compiling view position shader");
    shaders_m.push_back(Shader::fromFile("shaders/preRender.vertex.glsl", "shaders/preRender.fragment.glsl"));
    logger::INFO("Compiling SSR shader");
    shaders_m.push_back(Shader::fromFile("shaders/SSR.vertex.glsl", "shaders/SSRFragment.glsl"));

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    preRenderFramebufferId = 0;
    glGenFramebuffers(1, &preRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, preRenderFramebufferId);
    //texturePosId, textureNormalId, textureDepthId
    auto createTexture = [this](GLuint &textureId) {
        // The texture we're going to render to
        glGenTextures(1, &textureId);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Give an empty image to OpenGL ( the last "0" means "empty" )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_m, height_m, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Poor filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    };
    createTexture(texturePosId);
    createTexture(textureNormalId);
    createTexture(textureDepthId);


    // The depth buffer
    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_m, height_m);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texturePosId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureNormalId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textureDepthId, 0);
    // Set the list of draw buffers.
    GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        logger::ERROR("Frame buffer not created");

    sceneRenderFramebufferId = 0;
    glGenFramebuffers(1, &sceneRenderFramebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneRenderFramebufferId);
    createTexture(textureSceneId);

    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_m, height_m);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureSceneId, 0);
    // Set the list of draw buffers.
    GLenum SceneDrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(3, SceneDrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        logger::ERROR("Frame buffer not created");

}

void Renderer::setScene(const Scene &scene) {
    scene_m = scene;
}

void Renderer::renderScene(Shader &shader) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec2("viewPortSize", {width_m, height_m});
    shader.setMat4("invProj", glm::inverse(projection));
    scene_m.render(shader);
}


void Renderer::setShader(const Shader &shader) {
    shader_m = shader;
}

void Renderer::runMainLoop() {
    Mesh *mesh = scene_m.getMesh(0);
    while (!glfwWindowShouldClose(window_m)) {
        renderToTexture(shaders_m[1], width_m, height_m);
        renderSceneToTexture(shaders_m[0]);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width_m, height_m);
        postEffectScene(shaders_m[2]);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Dissolve window");
        ImGui::SliderFloat("spec", &roughness, 0.0001, 1.f);//temp sol
        ImGui::SliderFloat("metallic", &metallic, 0.0001, 1.f);
        ImGui::SliderFloat("strength", &strength, 0.0001, 1.f);
        shaders_m[2].use();
        shaders_m[2].setFloat("spec", roughness);
        shaders_m[2].setFloat("metallic", metallic);
        shaders_m[2].setFloat("strength", strength);
        ImGui::End();
        ImGui::Begin("Texture check");
        ImGui::Image((void *) (intptr_t) textureSceneId, ImVec2(width_m, height_m), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_m);
        glfwPollEvents();
    }
}

Renderer::~Renderer() {
    delete window_m;
}

void Renderer::renderToTexture(Shader &shader, size_t width, size_t height) {
// Set the list of draw buffers.
    shader.use();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textureDepthId);
    shader.setInt("depthTexture", 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texturePosId);
    shader.setInt("positionTexture", 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, textureNormalId);
    shader.setInt("normalTexture", 6);

    glBindFramebuffer(GL_FRAMEBUFFER, preRenderFramebufferId);
    glViewport(0, 0, width_m,
               height_m); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    renderScene(shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::renderSceneToTexture(Shader &shader) {
    shader.use();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textureSceneId);
    shader.setInt("tFrame", 4);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneRenderFramebufferId);
    glViewport(0, 0, width_m,
               height_m); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    renderScene(shader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::postEffectScene(Shader &shader) {
    shader.use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureDepthId);
    shader.setInt("tDepth", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureNormalId);
    shader.setInt("tNorm", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textureSceneId);
    shader.setInt("tFrame", 3);

    shader.setMat4("invView", glm::inverse(scene_m.getCamera().getViewMatrix()));
    shader.setMat4("view", scene_m.getCamera().getViewMatrix());
    shader.setMat4("proj", projection);
    shader.setMat4("invProj", glm::inverse(projection));
    renderScene(shader);
}


