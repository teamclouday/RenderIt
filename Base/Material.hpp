#pragma once

#include <memory>
#include <string>

#include "GLStructs.hpp"

/** @file */

namespace RenderIt
{

/// Material info
struct Material
{
    ~Material()
    {
        diffuse = specular = ambient = emissive = height = normals = shininess = opacity = displacement = nullptr;
        pbr_color = pbr_normal = pbr_emission = pbr_metalness = pbr_roughness = pbr_occlusion = nullptr;
    }

    bool HasPBR()
    {
        return pbr_color || pbr_normal || pbr_emission || pbr_metalness || pbr_roughness || pbr_occlusion;
    }

#pragma region material_maps

    // legacy materials
    std::shared_ptr<STexture> diffuse = nullptr;
    std::shared_ptr<STexture> specular = nullptr;
    std::shared_ptr<STexture> ambient = nullptr;
    std::shared_ptr<STexture> emissive = nullptr;
    std::shared_ptr<STexture> height = nullptr;
    std::shared_ptr<STexture> normals = nullptr;
    std::shared_ptr<STexture> shininess = nullptr;
    std::shared_ptr<STexture> opacity = nullptr;
    std::shared_ptr<STexture> displacement = nullptr;
    std::shared_ptr<STexture> lightmap = nullptr;
    std::shared_ptr<STexture> reflection = nullptr;

    inline static const std::string mapNameDiffuse = "map_DIFFUSE";
    inline static const std::string mapNameSpecular = "map_SPECULAR";
    inline static const std::string mapNameAmbient = "map_AMBIENT";
    inline static const std::string mapNameEmissive = "map_EMISSIVE";
    inline static const std::string mapNameHeight = "map_HEIGHT";
    inline static const std::string mapNameNormals = "map_NORMALS";
    inline static const std::string mapNameShininess = "map_SHININESS";
    inline static const std::string mapNameOpacity = "map_OPACITY";
    inline static const std::string mapNameDisplacement = "map_DISPLACEMENT";
    inline static const std::string mapNameLightmap = "map_LIGHTMAP";
    inline static const std::string mapNameReflection = "map_REFLECTION";

    // pbr materials
    std::shared_ptr<STexture> pbr_color = nullptr;
    std::shared_ptr<STexture> pbr_normal = nullptr;
    std::shared_ptr<STexture> pbr_emission = nullptr;
    std::shared_ptr<STexture> pbr_metalness = nullptr;
    std::shared_ptr<STexture> pbr_roughness = nullptr;
    std::shared_ptr<STexture> pbr_occlusion = nullptr;

    inline static const std::string mapNamePBRColor = "mapPBR_COLOR";
    inline static const std::string mapNamePBRNormal = "mapPBR_NORMAL";
    inline static const std::string mapNamePBREmission = "mapPBR_EMISSION";
    inline static const std::string mapNamePBRMetalness = "mapPBR_METALNESS";
    inline static const std::string mapNamePBRRoughness = "mapPBR_ROUGHNESS";
    inline static const std::string mapNamePBROcclusion = "mapPBR_OCCLUSION";

    // this line will be appended to any names
    // to indicate whether the corresponding texture exists
    // e.g. bool map_DIFFUSE_exists;
    inline static const std::string existsEXT = "_exists";

#pragma endregion material_maps

#pragma region material_consts

    glm::vec3 colorAmbient = glm::vec3(0.0f);
    glm::vec3 colorDiffuse = glm::vec3(0.0f);
    glm::vec3 colorSpecular = glm::vec3(0.0f);
    glm::vec3 colorEmissive = glm::vec3(0.0f);
    glm::vec3 colorTransparent = glm::vec3(0.0f);
    float valShininess = 0.0f;
    float valOpacity = 1.0f;
    float valRefract = 1.0f;

    inline static const std::string valNameColorAmbient = "val_AMBIENT";
    inline static const std::string valNameColorDiffuse = "val_DIFFUSE";
    inline static const std::string valNameColorSpecular = "val_SPECULAR";
    inline static const std::string valNameColorEmissive = "val_EMISSIVE";
    inline static const std::string valNameColorTransparent = "val_TRANSPARENT";
    inline static const std::string valNameValShininess = "val_SHININESS";
    inline static const std::string valNameValOpacity = "val_OPACITY";
    inline static const std::string valNameValRefract = "val_REFRACT";

#pragma endregion material_consts

#pragma region material_other

    bool twoSided = false;

#pragma endregion material_other
};

} // namespace RenderIt
