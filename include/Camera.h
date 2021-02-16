//
// Created by roundedglint585 on 2/14/21.
//

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H
#include <glm/glm.hpp>

class Camera {
public:
    Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);
    glm::mat4 getViewMatrix() const;
    glm::vec3 getCameraPosition() const;
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
};


#endif //PROJECT_CAMERA_H
