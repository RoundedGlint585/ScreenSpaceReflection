//
// Created by roundedglint585 on 2/16/21.
//

#include "../include/Scene.h"

void Scene::render(const Shader &shader) {
    shader.use();
    shader.setMat4("view", camera_m.getViewMatrix() );
    shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
    shader.setVec3("lightPosition", {10,5, 5.});
    shader.setVec3("camPos", camera_m.getCameraPosition());
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

Mesh* Scene::getMesh(size_t index) {
    if(index < meshes_m.size()){
        return &meshes_m[index];
    }
    return nullptr;
}

const Camera &Scene::getCamera() const {
    return camera_m;
}


