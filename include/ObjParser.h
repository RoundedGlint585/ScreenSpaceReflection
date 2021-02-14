//
// Created by roundedglint585 on 2/14/21.
//

#ifndef PROJECT_OBJPARSER_H
#define PROJECT_OBJPARSER_H
//
// Created by roundedglint585 on 8/3/19.
//

#pragma once
#include <sstream>
#include <vector>
#include <tuple>
#include <cstdint>
#include <unordered_map>
#include <glm/gtx/hash.hpp>
#include "Mesh.h"

namespace objParser {
    namespace details {
        std::array<std::vector<uint32_t>, 3> parseFaceString(std::string_view str) {
            std::array<std::vector<uint32_t>, 3> result = {}; // vertexIndex/normalIndex/textureIndex
            enum typeOfIndex {
                vertexIndex,
                normalIndex,
                textureIndex,
            };
            std::string num;
            uint32_t type = 0;
            for (auto &i: str) {
                if (i >= '0' && i <= '9') {
                    num += i;
                } else if (i == '/' || type == 2) {
                    if (type == 0) {
                        if(!num.empty()) {
                            result[vertexIndex].emplace_back(std::stoi(num));
                        }
                        type++;
                    } else if (type == 1) {
                        if(!num.empty()) {
                            result[textureIndex].emplace_back(std::stoi(num));
                        }
                        type++;
                    } else if (type == 2) {
                        if(!num.empty()) {
                            result[normalIndex].emplace_back(std::stoi(num));
                        }
                        type = 0;
                    }
                    num = "";
                }
            }
            if(!num.empty()){
                result[normalIndex].emplace_back(std::stoi(num));
            }
            return result;
        }


        template<class T> class PairHash;

        template<>
        class PairHash<std::tuple<glm::vec3, glm::vec3, glm::vec2>> {
        public:
            size_t operator()(const std::tuple<glm::vec3, glm::vec3, glm::vec2> &tuple) const {
                size_t h1 = std::hash<float>{}(std::get<0>(tuple).x);
                size_t h2 = std::hash<float>{}(std::get<1>(tuple).x);
                return h1 ^ (h2 << 1u);
            }
        };
        template<typename Stream>
        [[nodiscard]] inline std::tuple<std::vector<Mesh::Vertex>, std::vector<uint32_t>> getFromStream(Stream &&data){
            std::vector<glm::vec3> vertices, normals;
            std::vector<glm::vec2> textures;
            std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec2>> preMeshVertex;
            std::vector<uint32_t> indices = {};
            std::unordered_map<std::tuple<glm::vec3, glm::vec3, glm::vec2>, uint32_t, details::PairHash<std::tuple<glm::vec3,glm::vec3, glm::vec2>>> fromDataToIndex;

            std::string token;
            while (data >> token) {
                if (token == "v") {
                    float x, y, z;
                    data >> x >> y >> z;
                    vertices.emplace_back(x, y, z);
                } else if (token == "vn") {
                    float x, y, z;
                    data >> x >> y >> z;
                    normals.emplace_back(x, y, z);
                } else if (token == "f") { // todo: generate face as we should with indexed geometry
                    std::string toParse;
                    std::getline(data, toParse);
                    auto result = details::parseFaceString(toParse);
                    std::vector<uint32_t> parsedVertices = result[0];
                    std::vector<uint32_t> parsedNormals = result[1];
                    std::vector<uint32_t> parsedTextures = result[2];
                    auto findOrInsertPair = [&](uint32_t index){
                        auto possibleIndex = fromDataToIndex.find(
                                {vertices[parsedVertices[index] - 1], normals[parsedNormals[index] - 1], textures[parsedTextures[index]-1]});
                        if (possibleIndex == fromDataToIndex.end()) {
                            fromDataToIndex.insert({{vertices[parsedVertices[index] - 1], normals[parsedNormals[index] - 1], textures[parsedTextures[index]-1]},
                                                    preMeshVertex.size()});
                            indices.emplace_back(preMeshVertex.size());
                            preMeshVertex.emplace_back(vertices[parsedVertices[index] - 1], normals[parsedNormals[index] - 1], textures[parsedTextures[index]-1]);

                        } else {
                            indices.emplace_back((*possibleIndex).second);
                        }
                    };
                    for (size_t i = 0; i < 3; i++) {
                        findOrInsertPair(i);
                    }
                    if (parsedVertices.size() == 4) {
                        findOrInsertPair(0);
                        findOrInsertPair(2);
                        findOrInsertPair(3);
                    }
                } else if (token == "vt") {
                    float x, y;
                    data >> x >> y;
                    textures.emplace_back(x, y);
                } else if (token == "usemtl") {
                    continue; //todo: add materials
                } else if (token == "mtllib") {
                    continue; //todo: materials
                } else if (token == "o") {
                    data >> token;
                } else if (token == "g") {
                    continue; //todo: object group parsing
                } else {
                    continue;
                }
            }

            std::vector<Mesh::Vertex> verticesResult(preMeshVertex.size());
            for (size_t i = 0; i < preMeshVertex.size(); i++) {
                verticesResult[i] = {std::get<0>(preMeshVertex[i]), std::get<1>(preMeshVertex[i]), std::get<2>(preMeshVertex[i])};
            }
            return std::make_tuple(verticesResult, indices);
        }

    }


    [[nodiscard]] inline std::tuple<std::vector<Mesh::Vertex>, std::vector<uint32_t>> parseData(const std::string& objData){
        std::stringstream data(objData);
        return details::getFromStream(std::move(data));
    }

    [[nodiscard]] inline std::tuple<std::vector<Mesh::Vertex>, std::vector<uint32_t>> parseFile(std::string_view path) {
        std::ifstream file(&path.front());
        return details::getFromStream(std::move(file));
    }
}

#endif //PROJECT_OBJPARSER_H
