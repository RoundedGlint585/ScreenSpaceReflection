//
// Created by roundedglint585 on 8/3/19.
//

#include "../include/Mesh.h"

#include <utility>

Mesh::Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex> vertices) : vertices_(std::move(vertices)),
                                                                                indices_(std::move(indices)), VAO_(-1), VBO_(-1), EBO_(-1) {
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
    shader.setFloat("metallic", material_.metallic);
    shader.setFloat("roughness", material_.roughness);
    shader.setMat4("model", getModelMatrix());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    shader.setInt("texture1", 0); // or with shader class
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


void Mesh::setTexture(const uint8_t *data, size_t width, size_t height) {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);

}

glm::mat4 Mesh::getModelMatrix() const {
    //placeholder for now, maybe implement transform later
    return glm::mat4(1.f);
}

Mesh::Material Mesh::getMaterial() const {
    return material_;
}

void Mesh::updateMaterial(float roughness, float metallic) {
    material_.roughness = roughness;
    material_.metallic = metallic;

}

GLuint Mesh::getTexture() const {
    return texture_;
}
