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

public:

    Mesh(std::vector<uint32_t> indices, std::initializer_list<std::vector<Mesh::Vertex>> list);

    Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex>  vertices);
    Mesh() = default;

    void draw(const Shader &shader) const;

private:
    void initMesh();

    uint32_t VAO_, VBO_, EBO_;
    std::vector<Mesh::Vertex> vertices_;
    std::vector<uint32_t> indices_;
    uint32_t morphPositionCount;
};


#endif //OPENGL_MESH_H
