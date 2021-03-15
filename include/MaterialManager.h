//
// Created by roundedglint585 on 3/15/21.
//

#ifndef PROJECT_MATERIALMANAGER_H
#define PROJECT_MATERIALMANAGER_H


#include <unordered_map>
#include <string>
#include <GL/glew.h>

struct Material{
    size_t baseColorTexture;
    size_t metallicTexture;
    size_t normalTexture;
    size_t roughnessTexture;
    int width, height;
};
class MaterialManager {
public:
    //singleton class, bad practise, but I need centralized storage for textures;
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;
    MaterialManager(MaterialManager&&) = delete;
    MaterialManager& operator=(MaterialManager&&) = delete;

    static MaterialManager& getInstance();
    Material& addMaterial(const std::string& name);
    const Material& getMaterial(const std::string& name);
private:
    MaterialManager() = default;
    std::unordered_map<std::string, Material> materials_;
};

#endif //PROJECT_MATERIALMANAGER_H
