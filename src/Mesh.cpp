//
// Created by roundedglint585 on 8/3/19.
//

#include "../include/Mesh.h"

#include <utility>

Mesh::Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex> vertices) : vertices_(std::move(vertices)),
                                                                                indices_(std::move(indices)), VAO_(-1),
                                                                                VBO_(-1), EBO_(-1) {
    initMesh();
}


void Mesh::initMesh() {
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
                 indices_.data(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, normal)));
    // vertex texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, texture)));

    glBindVertexArray(0);

}

void Mesh::draw(const Shader &shader) const {
    shader.use();
    shader.setMat4("model", getModelMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material_.baseColorTexture);
    shader.setInt("textureBaseColor", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material_.normalTexture);
    shader.setInt("textureNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, material_.roughnessTexture);
    shader.setInt("textureRoughness", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, material_.metallicTexture);
    shader.setInt("textureMetallic", 3);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


glm::mat4 Mesh::getModelMatrix() const {
    //placeholder for now, maybe implement transform later
    return glm::mat4(1.f);
}


void Mesh::setMaterial(const Material &material) {
    material_ = material;
}

const Material &Mesh::getMaterial() const {
    return material_;
}

