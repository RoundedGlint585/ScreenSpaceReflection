#include <GL/glew.h>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "include/MaterialManager.h"
#include "include/Mesh.h"
#include "include/ObjParser.h"
#include "include/Camera.h"
#include "include/Scene.h"
#include "include/Renderer.h"
#include "include/Logger.h"

int main() {
    logger::INFO("Loading shaders and data");
    Renderer renderer;
    renderer.init(1000, 1000);

    MaterialManager& manager = MaterialManager::getInstance();
    manager.addMaterial("Metal");
    manager.addMaterial("Wood");

    auto [vertices1, indices1] = objParser::parseFile("objects/teapot.obj");
    Mesh mesh1 = Mesh(indices1, vertices1);
    auto [vertices2, indices2] = objParser::parseFile("objects/ground.obj");
    Mesh mesh2 = Mesh(indices2, vertices2);
    auto [vertices3, indices3] = objParser::parseFile("objects/sphere.obj");
    Mesh mesh3 = Mesh(indices3, vertices3);
    auto [vertices4, indices4] = objParser::parseFile("objects/cube2.obj");
    Mesh mesh4 = Mesh(indices4, vertices4);
    Camera camera({-12,10, 8}, {0, 0, 0}, {0.f, 1.f, 0});
    mesh1.setMaterial(manager.getMaterial("Wood"));
    mesh3.setMaterial(manager.getMaterial("Wood"));
    mesh4.setMaterial(manager.getMaterial("Wood"));
    mesh2.setMaterial(manager.getMaterial("Metal"));

    Scene scene;
    scene.setCamera(camera);
    scene.addMesh(mesh1);
    scene.addMesh(mesh2);
    scene.addMesh(mesh3);
    scene.addMesh(mesh4);

    logger::INFO("Initiating main loop");
    renderer.setScene(scene);
    logger::INFO("Starting main loop");
    renderer.runMainLoop();
    logger::INFO("Exiting");

    return 0;
}
