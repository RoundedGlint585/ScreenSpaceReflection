#include <GLFW/glfw3.h>
#include "include/Shader.h"
#include "include/Mesh.h"
#include "include/ObjParser.h"
#include "include/Camera.h"
#include "include/Scene.h"
#include "include/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {

    Renderer renderer;
    renderer.init(512, 512);
    Shader shader = Shader::fromFile("shaders/baseVertex.glsl", "shaders/baseFragment.glsl");
    auto [vertices, indices] = objParser::parseFile("objects/cube.obj");
    Mesh mesh = Mesh(indices, vertices);
    Camera camera({3,0, 5.}, {0, 0, 0}, {0.f, 1.f, 0});
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load("textures/wall.jpg", &width, &height, &nrChannels, 0);
    mesh.setTexture(data, width, height);

    Scene scene;
    scene.setCamera(camera);
    scene.addMesh(mesh);
    renderer.setScene(scene);
    renderer.setShader(shader);
    renderer.runMainLoop();

    return 0;
}
