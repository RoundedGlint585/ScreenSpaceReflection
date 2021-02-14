//
// Created by roundedglint585 on 2/14/21.
//

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up):position(position), direction(glm::normalize(direction - position)), up(glm::normalize(up)) {

}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, direction, up);
}
