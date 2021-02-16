//
// Created by roundedglint585 on 2/16/21.
//

#include "Scene.h"

void Scene::render(const Shader &shader) {
    shader.use();
    shader.setMat4("view", camera_m.getViewMatrix() );
    shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
    shader.setVec3("lightPos", {3.f, 0.f, 5.f});
    shader.setVec3("cameraPos", camera_m.getCameraPosition());
    for(auto &mesh: meshes_m){
        mesh.draw(shader);
    }
}

void Scene::addMesh(const Mesh &mesh) {
    meshes_m.emplace_back(mesh);
}

void Scene::setCamera(const Camera &camera) {
    camera_m = camera;
}
