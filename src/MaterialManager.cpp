//
// Created by roundedglint585 on 3/15/21.
//
#include "../include/MaterialManager.h"
#include "../include/Logger.h"
#include "stb_image.h"

MaterialManager &MaterialManager::getInstance() {
    static MaterialManager manager;
    return manager;
}

Material& MaterialManager::addMaterial(const std::string &name) {
    std::array<std::string, 4> texturesName = {"BaseColor.png", "Metallic.png", "Normal.png", "Roughness.png"};
    std::array<GLuint, 4> texturesIndexes = {};
    std::array<GLuint, 4> textureType = {GL_RGB, GL_RED, GL_RGB, GL_RED};
    int width, height, nrChannels;
    uint8_t *data = nullptr;
    stbi_set_flip_vertically_on_load(true);
    for(size_t i = 0; i < texturesName.size(); i++){
        std::string path = "textures/" + name + "/" + texturesName[i];
        logger::INFO("Loading " + name + " material: " + texturesName[i]);
        data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if(!data){
            logger::ERROR("texture with name: " + path + " was not loaded");
        }
        glGenTextures(1, &texturesIndexes[i]);
        glBindTexture(GL_TEXTURE_2D, texturesIndexes[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, textureType[i], width, height, 0, textureType[i], GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        stbi_image_free(data);
        data = nullptr;
    }
    materials_.emplace(name, Material{texturesIndexes[0], texturesIndexes[1], texturesIndexes[2], texturesIndexes[3], width, height});
    return materials_[name];
}

const Material &MaterialManager::getMaterial(const std::string &name) {
    return materials_[name];
}
