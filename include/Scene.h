//
// Created by roundedglint585 on 2/16/21.
//

#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H


#include <vector>
#include <optional>
#include "Mesh.h"
#include "Camera.h"

class Scene {
public:
    Scene() = default;
    void render(const Shader &shader);

    void addMesh(const Mesh &mesh);

    void setCamera(const Camera &camera);
    const Camera& getCamera() const;

    Mesh* getMesh(size_t index);
private:
    std::vector<Mesh> meshes_m;
    Camera camera_m;
};


#endif //PROJECT_SCENE_H
