#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "include/Mesh.h"
#include "include/ObjParser.h"
#include "include/Camera.h"
#include "include/Scene.h"
#include "include/Renderer.h"
#include "include/Logger.h"
#include "include/Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
    logger::INFO("Loading shaders and data");
    Renderer renderer;
    renderer.init(800, 600);
    auto [vertices1, indices1] = objParser::parseFile("objects/cube2.obj");
    Mesh mesh1 = Mesh(indices1, vertices1);
    auto [vertices2, indices2] = objParser::parseFile("objects/ground.obj");
    Mesh mesh2 = Mesh(indices2, vertices2);
    Camera camera({10,0, 5.}, {0, 0, 0}, {0.f, 1.f, 0});
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load("textures/red.jpg", &width, &height, &nrChannels, 0);
    mesh1.setTexture(data, width, height);
    stbi_image_free(data);
    data = stbi_load("textures/ground.jpg", &width, &height, &nrChannels, 0);
    mesh2.setTexture(data, width, height);
    stbi_image_free(data);
    mesh2.updateMaterial(0.35, 0.655);
    Scene scene;
    scene.setCamera(camera);
    scene.addMesh(mesh2);
    scene.addMesh(mesh1);
    logger::INFO("Initiating main loop");
    renderer.setScene(scene);

    logger::INFO("Starting main loop");
    renderer.runMainLoop();
    logger::INFO("Exiting");

    return 0;
}
