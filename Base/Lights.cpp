#include "Lights.hpp"

#include <cstring>

namespace RenderIt
{

LightManager::LightManager()
    : lightDrawScale(0.05f), drawDirLights(true), drawPointLights(true), drawSpotLights(true),
      _dirLightsSSBOUpdated(false), _pointLightsSSBOUpdated(false), _spotLightsSSBOUpdated(false)
{
    _dirLights.reserve(LIGHTS_MAX_DIR_LIGHTS);
    _pointLights.reserve(LIGHTS_MAX_POINT_LIGHTS);
    _spotLights.reserve(LIGHTS_MAX_SPOT_LIGHTS);

    _lightsSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _lightsSSBO->Bind();
    glBufferData(_lightsSSBO->type,
                 LIGHTS_MAX_DIR_LIGHTS * sizeof(DirLight) + LIGHTS_MAX_POINT_LIGHTS * sizeof(PointLight) +
                     LIGHTS_MAX_SPOT_LIGHTS * sizeof(SpotLight) + 3 * sizeof(unsigned),
                 nullptr, GL_DYNAMIC_DRAW);
    _lightsSSBO->UnBind();

    prepareDrawData();
}

std::shared_ptr<LightManager> LightManager::Instance()
{
    static auto manager = std::make_shared<LightManager>();
    return manager;
}

void LightManager::Update(bool updateAllLights)
{
    _dirLightsSSBOUpdated &= !updateAllLights;
    _pointLightsSSBOUpdated &= !updateAllLights;
    _spotLightsSSBOUpdated &= !updateAllLights;
    updateSSBO();
}

void LightManager::DrawLights(const glm::mat4 &matProjView, const glm::vec3 &cameraPos) const
{
    std::shared_ptr<Mesh> mesh = _drawModel->GetMesh(0);
    if (!mesh)
        return;
    _drawShader->Bind();
    _drawShader->UniformMat4("mProjView", matProjView);
    _drawShader->UniformFloat("vLightScale", lightDrawScale);
    _drawShader->UniformVec3("vCameraPos", cameraPos);
    BindLights(0);
    auto VAO = mesh->GetVertexArray().value();
    auto count = mesh->GetNumIndices();
    glBindVertexArray(VAO);
    // dir lights
    if (drawDirLights && !_dirLights.empty() && mesh)
    {
        auto hasDepth = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        _drawShader->UniformInt("vLightType", 0);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(_dirLights.size()));
        if (hasDepth)
            glEnable(GL_DEPTH_TEST);
    }
    if (drawPointLights && !_pointLights.empty() && mesh)
    {
        _drawShader->UniformInt("vLightType", 1);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(_pointLights.size()));
    }
    if (drawSpotLights && !_spotLights.empty() && mesh)
    {
        _drawShader->UniformInt("vLightType", 2);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, 0,
                                static_cast<GLsizei>(_spotLights.size()));
    }
    glBindVertexArray(0);
    UnBindLights(0);
    _drawShader->UnBind();
}

void LightManager::BindLights(unsigned binding) const
{
    _lightsSSBO->BindBase(binding);
}

void LightManager::UnBindLights(unsigned binding) const
{
    _lightsSSBO->UnBindBase(binding);
}

bool LightManager::PushLight(LightType type)
{
    switch (type)
    {
    case LightType::Point: {
        if (_pointLights.size() < LIGHTS_MAX_POINT_LIGHTS)
        {
            _pointLights.push_back(PointLight());
            _pointLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Spot: {
        if (_spotLights.size() < LIGHTS_MAX_SPOT_LIGHTS)
        {
            _spotLights.push_back(SpotLight());
            _spotLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Directional:
    default: {
        if (_dirLights.size() < LIGHTS_MAX_DIR_LIGHTS)
        {
            _dirLights.push_back(DirLight());
            _dirLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    }
}

bool LightManager::PopLight(LightType type)
{
    switch (type)
    {
    case LightType::Point: {
        if (!_pointLights.empty())
        {
            _pointLights.pop_back();
            _pointLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Spot: {
        if (!_spotLights.empty())
        {
            _spotLights.pop_back();
            _spotLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Directional:
    default: {
        if (!_dirLights.empty())
        {
            _dirLights.pop_back();
            _dirLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    }
}

bool LightManager::RemoveLight(LightType type, unsigned idx)
{
    switch (type)
    {
    case LightType::Point: {
        if (idx < _pointLights.size())
        {
            _pointLights.erase(_pointLights.begin() + idx);
            _pointLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Spot: {
        if (idx < _spotLights.size())
        {
            _spotLights.erase(_spotLights.begin() + idx);
            _spotLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    case LightType::Directional:
    default: {
        if (idx < _dirLights.size())
        {
            _dirLights.erase(_dirLights.begin() + idx);
            _dirLightsSSBOUpdated = false;
            return true;
        }
        else
            return false;
    }
    }
}

void *LightManager::GetLight(LightType type, unsigned idx)
{
    switch (type)
    {
    case LightType::Point: {
        if (idx < _pointLights.size())
            return &_pointLights[idx];
        else
            return nullptr;
    }
    case LightType::Spot: {
        if (idx < _spotLights.size())
            return &_spotLights[idx];
        else
            return nullptr;
    }
    case LightType::Directional:
    default: {
        if (idx < _dirLights.size())
            return &_dirLights[idx];
        else
            return nullptr;
    }
    }
}

size_t LightManager::GetNumLights(LightType type) const
{
    switch (type)
    {
    case LightType::Point:
        return _pointLights.size();
    case LightType::Spot:
        return _spotLights.size();
    case LightType::Directional:
    default:
        return _dirLights.size();
    }
}

void LightManager::updateSSBO()
{
    if (!_dirLightsSSBOUpdated)
    {
        auto offset = 0;
        _lightsSSBO->Bind();
        auto sizePtr = reinterpret_cast<unsigned *>(
            glMapBufferRange(_lightsSSBO->type, offset, sizeof(unsigned), GL_MAP_WRITE_BIT));
        *sizePtr = static_cast<unsigned>(_dirLights.size());
        glUnmapBuffer(_lightsSSBO->type);
        if (!_dirLights.empty())
        {
            auto numBytes = _dirLights.size() * sizeof(DirLight);
            auto dataPtr = glMapBufferRange(_lightsSSBO->type, offset + sizeof(unsigned), numBytes, GL_MAP_WRITE_BIT);
            std::memcpy(dataPtr, _dirLights.data(), numBytes);
            glUnmapBuffer(_lightsSSBO->type);
        }
        _lightsSSBO->UnBind();
        _dirLightsSSBOUpdated = true;
    }
    if (!_pointLightsSSBOUpdated)
    {
        auto offset = sizeof(unsigned) + LIGHTS_MAX_DIR_LIGHTS * sizeof(DirLight);
        _lightsSSBO->Bind();
        auto sizePtr = reinterpret_cast<unsigned *>(
            glMapBufferRange(_lightsSSBO->type, offset, sizeof(unsigned), GL_MAP_WRITE_BIT));
        *sizePtr = static_cast<unsigned>(_pointLights.size());
        glUnmapBuffer(_lightsSSBO->type);
        if (!_pointLights.empty())
        {
            auto numBytes = _pointLights.size() * sizeof(PointLight);
            auto dataPtr = glMapBufferRange(_lightsSSBO->type, offset + sizeof(unsigned), numBytes, GL_MAP_WRITE_BIT);
            std::memcpy(dataPtr, _pointLights.data(), numBytes);
            glUnmapBuffer(_lightsSSBO->type);
        }
        _lightsSSBO->UnBind();
        _pointLightsSSBOUpdated = true;
    }
    if (!_spotLightsSSBOUpdated)
    {
        auto offset = 2 * sizeof(unsigned) + LIGHTS_MAX_DIR_LIGHTS * sizeof(DirLight) +
                      LIGHTS_MAX_POINT_LIGHTS * sizeof(PointLight);
        _lightsSSBO->Bind();
        auto sizePtr = reinterpret_cast<unsigned *>(
            glMapBufferRange(_lightsSSBO->type, offset, sizeof(unsigned), GL_MAP_WRITE_BIT));
        *sizePtr = static_cast<unsigned>(_spotLights.size());
        glUnmapBuffer(_lightsSSBO->type);
        if (!_spotLights.empty())
        {
            auto numBytes = _spotLights.size() * sizeof(SpotLight);
            auto dataPtr = glMapBufferRange(_lightsSSBO->type, offset + sizeof(unsigned), numBytes, GL_MAP_WRITE_BIT);
            std::memcpy(dataPtr, _spotLights.data(), numBytes);
            glUnmapBuffer(_lightsSSBO->type);
        }
        _lightsSSBO->UnBind();
        _spotLightsSSBOUpdated = true;
    }
}

void LightManager::prepareDrawData()
{
    // load shaders
    std::string vertSource = R"(
        #version 450 core
        layout(location = 0) in vec3 inPos;
        layout(location = 0) out vec3 vertColor;

        uniform mat4 mProjView;
        uniform int vLightType;
        uniform float vLightScale;
        uniform vec3 vCameraPos;

        struct DirLight
        {
            float dir[3];
            float color[3];
            float intensity;
            int castShadow;
            float shadowStrength;
        };

        struct PointLight
        {
            float pos[3];
            float range;
            float color[3];
            float intensity;
            int castShadow;
            float shadowStrength;
        };

        struct SpotLight
        {
            float pos[3];
            float dir[3];
            float range;
            float cutoffInner;
            float cutoffOuter;
            float color[3];
            float intensity;
            int castShadow;
            float shadowStrength;
        };

        #define LIGHTS_MAX_DIR_LIGHTS 4
        #define LIGHTS_MAX_POINT_LIGHTS 4
        #define LIGHTS_MAX_SPOT_LIGHTS 4

        layout(std430, binding = 0) buffer LightsData
        {
            uint dirLightsLen;
            DirLight dirLights[LIGHTS_MAX_DIR_LIGHTS];
            uint pointLightsLen;
            PointLight pointLights[LIGHTS_MAX_POINT_LIGHTS];
            uint spotLightsLen;
            SpotLight spotLights[LIGHTS_MAX_SPOT_LIGHTS];
        };

        #define LIGHT_TYPE_DIR 0
        #define LIGHT_TYPE_POINT 1
        #define LIGHT_TYPE_SPOT 2

        void main()
        {
            vec4 pos = vec4(vLightScale * inPos, 1.0);
            if (vLightType == LIGHT_TYPE_DIR)
            {
                DirLight light = dirLights[gl_InstanceID];
                pos.xyz += vCameraPos + vec3(light.dir[0], light.dir[1], light.dir[2]);
                vertColor = vec3(light.color[0], light.color[1], light.color[2]) * light.intensity;
            }
            else if (vLightType == LIGHT_TYPE_POINT)
            {
                PointLight light = pointLights[gl_InstanceID];
                pos.xyz += vec3(light.pos[0], light.pos[1], light.pos[2]);
                vertColor = vec3(light.color[0], light.color[1], light.color[2]) * light.intensity;
            }
            else
            {
                SpotLight light = spotLights[gl_InstanceID];
                pos.xyz += vec3(light.pos[0], light.pos[1], light.pos[2]);
                vertColor = vec3(light.color[0], light.color[1], light.color[2]) * light.intensity;
            }
            gl_Position = mProjView * pos;
        }
    )";
    std::string fragSource = R"(
        #version 450 core
        layout(location = 0) out vec4 outColor;
        layout(location = 0) in vec3 vertColor;
        void main(){outColor = vec4(vertColor, 1.0);}
    )";
    _drawShader = std::make_unique<Shader>();
    _drawShader->AddSource(vertSource, GL_VERTEX_SHADER);
    _drawShader->AddSource(fragSource, GL_FRAGMENT_SHADER);
    _drawShader->Compile();
    // load models
    _drawModel = std::make_unique<Model>();
    _drawModel->Load(MeshShape::Cube);
}

} // namespace RenderIt
