//
// Created by roundedglint585 on 8/3/19.
//

#include "../include/Mesh.h"

#include <utility>

Mesh::Mesh(std::vector<uint32_t> indices, std::vector<Mesh::Vertex> vertices) : vertices_(std::move(vertices)),
                                                                                indices_(std::move(indices)), VAO_(-1), VBO_(-1), EBO_(-1), morphPositionCount(1) {
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
    for(size_t i = 0; i < morphPositionCount; i++) {
        // vertex positions
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, morphPositionCount*sizeof(Vertex), (void *)(sizeof(Vertex)*i));
        // vertex normals
        glEnableVertexAttribArray(i+1);
        glVertexAttribPointer(i + 1, 3, GL_FLOAT, GL_FALSE, morphPositionCount*sizeof(Vertex), (void *) (sizeof(Vertex)*i+ offsetof(Vertex, normal)));
        // vertex texture
        glEnableVertexAttribArray(i+2);
        glVertexAttribPointer(i + 2, 2, GL_FLOAT, GL_FALSE, morphPositionCount*sizeof(Vertex), (void *) (sizeof(Vertex)*i+ offsetof(Vertex, texture)));
    }
    glBindVertexArray(0);

}

void Mesh::draw(const Shader &shader) const {
    shader.use();
    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh::Mesh(std::vector<uint32_t> indices, std::initializer_list<std::vector<Mesh::Vertex>> list) : indices_(std::move(indices)), VAO_(-1), VBO_(-1), EBO_(-1), morphPositionCount(list.size())  {
    uint32_t verticesCount = list.begin()->size();
    for(size_t i = 0; i < verticesCount; i++){
        for(auto& el: list){
            vertices_.emplace_back(el[i]);
        }
    }
    initMesh();
}
