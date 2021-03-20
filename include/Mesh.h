//
// Created by roundedglint585 on 8/3/19.
//

#ifndef OPENGL_MESH_H
#define OPENGL_MESH_H
#define STB_IMAGE_IMPLEMENTATION
#include <vector>
#include <GL/glew.h>
#include <cstddef>
#include "Shader.h"
#include "MaterialManager.h"
//now by default we will have normal, comparingBias, rayStep cass
class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texture;
    };

public:

    Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex> vertices);
    Mesh() = default;

    void draw(const Shader &shader) const;
    void setMaterial(const Material &material);
    const Material& getMaterial() const;

    glm::mat4 getModelMatrix() const;

private:
    void initMesh();

    uint32_t VAO_, VBO_, EBO_;
    std::vector<Mesh::Vertex> vertices_;
    std::vector<uint32_t> indices_;
    uint32_t texture_, metallic_;
    Material material_;
};


#endif //OPENGL_MESH_H
