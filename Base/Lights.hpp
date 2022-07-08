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

/** @file */

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
              float lCutoffIn = 0.7f, float lCutoffOut = 0.8f, const glm::vec3 &lColor = glm::vec3(1.0f),
              float lIntensity = 1.0f, bool lCastShadow = false)
    {
        pos = lPos;
        dir = lDir;
        range = lRange;
        cutoffInner = lCutoffIn;
        cutoffOuter = lCutoffOut;
        color = lColor;
        intensity = lIntensity;
        castShadow = static_cast<int>(lCastShadow);
    }

    glm::vec3 pos;
    glm::vec3 dir;
    float range;
    float cutoffInner;
    float cutoffOuter;
    glm::vec3 color;
    float intensity;
    int castShadow;
};

/// Light type
enum class LightType
{
    Directional,
    Point,
    Spot,
    Area
};

/// Global light manager
class LightManager
{
    friend class Shadow;

  public:
    LightManager();

    /// Get instance
    static std::shared_ptr<LightManager> Instance();

    /// Update light data (SSBO)
    void Update(bool updateAllLights = false);

    /// Bind light data
    void BindLights(unsigned binding = 0) const;

    /// Unbine light data
    void UnBindLights(unsigned binding = 0) const;

    /// Push new light of given type
    bool PushLight(LightType type);

    /// Pop last light of type
    bool PopLight(LightType type);

    /// Remove light at index
    bool RemoveLight(LightType type, unsigned idx);

    // IMPORTANT: this method provides reference, but SSBOs won't auto-update
    // To update SSBOs, please Update(updateAllLights = true)

    /// Get light at index
    template <typename T> std::optional<T &> GetLight(LightType type, unsigned idx);

    /// Get number of lights of type
    size_t GetNumLights(LightType type) const;

    /// UI calls
    void UI();

  public:
    const std::string LOGNAME = "LightManager";

  private:
    /// Update lights SSBO
    void updateSSBO();

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
