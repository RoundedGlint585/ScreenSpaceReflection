//
// Created by roundedglint585 on 2/16/21.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H


#include <vector>
#include "include/Mesh.h"
#include "include/Camera.h"

class Scene {
public:
    Scene() = default;
    void render(const Shader &shader);

    void addMesh(const Mesh &mesh);

    void setCamera(const Camera &camera);
private:
    std::vector<Mesh> meshes_m;
    Camera camera_m;

};


#endif //PROJECT_SCENE_H
