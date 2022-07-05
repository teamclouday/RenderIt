#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "GLStructs.hpp"
#include "Shader.hpp"

#define LIGHTS_MAX_DIR_LIGHTS 4
#define LIGHTS_MAX_POINT_LIGHTS 4
#define LIGHTS_MAX_SPOT_LIGHTS 4
#define LIGHTS_SHADOW_MAP_SIZE 256

namespace RenderIt
{

/// Directional Light
struct DirLight
{
    DirLight(const glm::vec3 &lDir = glm::vec3(1.0f), const glm::vec3 &lColor = glm::vec3(1.0f),
             float lIntensity = 1.0f, bool lCastShadow = false)
    {
        dir = lDir;
        color = lColor;
        intensity = lIntensity;
        castShadow = static_cast<int>(lCastShadow);
    }

    glm::vec3 dir;
    glm::vec3 color;
    float intensity;
    int castShadow;
};

/// Point Light
struct PointLight
{
    PointLight(const glm::vec3 &lPos = glm::vec3(0.0f), float lRange = 1.0f, const glm::vec3 &lColor = glm::vec3(1.0f),
               float lIntensity = 1.0f, bool lCastShadow = false)
    {
        pos = lPos;
        range = lRange;
        color = lColor;
        intensity = lIntensity;
        castShadow = static_cast<int>(lCastShadow);
    }

    glm::vec3 pos;
    float range;
    glm::vec3 color;
    float intensity;
    int castShadow;
};

/// Spot Light
struct SpotLight
{
    SpotLight(const glm::vec3 &lPos = glm::vec3(0.0f), const glm::vec3 &lDir = glm::vec3(-1.0f), float lRange = 1.0f,
              float lCutoff = 0.8f, const glm::vec3 &lColor = glm::vec3(1.0f), float lIntensity = 1.0f,
              bool lCastShadow = false)
    {
        pos = lPos;
        dir = lDir;
        range = lRange;
        cutoff = lCutoff;
        color = lColor;
        intensity = lIntensity;
        castShadow = static_cast<int>(lCastShadow);
    }

    glm::vec3 pos;
    glm::vec3 dir;
    float range;
    float cutoff;
    glm::vec3 color;
    float intensity;
    int castShadow;
};

enum class LightType
{
    Directional,
    Point,
    Spot
};

/// Global light manager
class LightManager
{
    friend class Shadow;

  public:
    LightManager();

    static std::shared_ptr<LightManager> Instance();

    void Update(bool updateAllLights = false);

    void BindLights(unsigned binding = 0) const;

    void UnBindLights(unsigned binding = 0) const;

    bool PushLight(LightType type);

    bool PopLight(LightType type);

    bool RemoveLight(LightType type, unsigned idx);

    // IMPORTANT: this method provides reference, but SSBOs won't auto-update
    // To update SSBOs, please Update(updateAllLights = true)

    template <typename T> std::optional<T &> GetLight(LightType type, unsigned idx);

    size_t GetNumLights(LightType type) const;

    void UI();

  public:
    const std::string NAME = "LightManager";

  private:
    void updateSSBOs();

  private:
    // light data
    std::vector<DirLight> _dirLights;
    std::vector<PointLight> _pointLights;
    std::vector<SpotLight> _spotLights;

    // light data SSBO
    // layout:
    // (size, dir data, size, point data, size, spot data)
    std::unique_ptr<SBuffer> _lightsSSBO;
    bool _dirLightsSSBOUpdated;
    bool _pointLightsSSBOUpdated;
    bool _spotLightsSSBOUpdated;

    // light shadow maps
    std::unique_ptr<STexture> _dirLightsShadowMaps;
    std::unique_ptr<STexture> _pointLightsShadowMaps;
    std::unique_ptr<STexture> _spotLightsShadowMaps;
};

} // namespace RenderIt
