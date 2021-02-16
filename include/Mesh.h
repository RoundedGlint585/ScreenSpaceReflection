//
// Created by roundedglint585 on 8/3/19.
//

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H

#include <vector>
#include <GL/glew.h>
#include <cstddef>
#include "Shader.h"

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;
    };
    struct Material {
        float roughness = 0.1;
        float metallic = 0.1;
    };

public:

    Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex>  vertices);
    Mesh() = default;

    void draw(const Shader &shader) const;
    void setTexture(const uint8_t *data, size_t width, size_t height);

    glm::mat4 getModelMatrix() const;
    Material getMaterial() const;

    void updateMaterial(float roughness, float metallic);
private:
    void initMesh();

    uint32_t VAO_, VBO_, EBO_;
    std::vector<Mesh::Vertex> vertices_;
    std::vector<uint32_t> indices_;
    uint32_t texture_;
    Material material_;
};


#endif //OPENGL_MESH_H
