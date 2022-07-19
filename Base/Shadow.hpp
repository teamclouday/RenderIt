#pragma once
#include <array>
#include <functional>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "Camera.hpp"
#include "GLStructs.hpp"
#include "Lights.hpp"
#include "Shader.hpp"

#define SHADOW_SIZE 1024

namespace RenderIt
{

class ShadowManager
{
  public:
    ShadowManager();

    static std::shared_ptr<ShadowManager> Instance();

    void AttachCamera(std::shared_ptr<Camera> camera);

    void AttachLightManager(std::shared_ptr<LightManager> lights);

    void RecordShadows(std::function<void(const Shader *)> renderFunc);

    GLuint GetShadowMaps(LightType type) const;

    void BindShadowData(LightType type, unsigned binding) const;

    void BindLightSpaceData(LightType type, unsigned binding) const;

    void UI();

  public:
    const std::string NAME = "ShadowManager";
    const std::string ShaderModelName = "mat_Model";

  private:
#pragma region cascaded_shadow

    void setupCSM();

    void setupCSMBuffers();

    void setupCSMShaders();

    void computeCSMLightMatrices();

#pragma endregion cascaded_shadow

#pragma region omnidirectional_shadow

    void setupOmni();

    void setupOmniBuffers();

    void setupOmniShaders();

    void computeOmniLightMatrices();

#pragma endregion omnidirectional_shadow

  private:
    std::shared_ptr<LightManager> _lights;
    std::shared_ptr<Camera> _camera;

#pragma region cascaded_shadow
    float _csmNearOffset;
    glm::vec2 _csmOffsets;
    std::unique_ptr<STexture> _csmShadowMaps;
    std::unique_ptr<SFBO> _csmFBO;
    std::shared_ptr<Shader> _csmShader;
    std::shared_ptr<SBuffer> _csmSSBO;
#pragma endregion cascaded_shadow

#pragma region omnidirectional_shadow
    glm::vec2 _omniOffsets;
    std::unique_ptr<STexture> _omniShadowMaps;
    std::shared_ptr<SFBO> _omniFBO;
    std::shared_ptr<Shader> _omniShader;
    std::shared_ptr<SBuffer> _omniSSBO;
#pragma endregion omnidirectional_shadow
};

} // namespace RenderIt
