#include "Lights.hpp"

#include <cstring>
#include <typeinfo>

namespace RenderIt
{

LightManager::LightManager()
    : _dirLightsSSBOUpdated(false), _pointLightsSSBOUpdated(false), _spotLightsSSBOUpdated(false)
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
    updateSSBOs();
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

template <typename T> std::optional<T &> LightManager::GetLight(LightType type, unsigned idx)
{
    switch (type)
    {
    case LightType::Point: {
        if (typeid(T) == typeid(PointLight) && idx < _pointLights.size())
            return std::optional<std::reference_wrapper<PointLight>>{_pointLights[idx]};
        else
            return std::nullopt;
    }
    case LightType::Spot: {
        if (typeid(T) == typeid(SpotLight) && idx < _spotLights.size())
            return std::optional<std::reference_wrapper<SpotLight>>{_spotLights[idx]};
        else
            return std::nullopt;
    }
    case LightType::Directional:
    default: {
        if (typeid(T) == typeid(DirLight) && idx < _dirLights.size())
            return std::optional<std::reference_wrapper<DirLight>>{_dirLights[idx]};
        else
            return std::nullopt;
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

void LightManager::updateSSBOs()
{
    if (!_dirLightsSSBOUpdated)
    {
        auto offset = 0;
        _lightsSSBO->Bind();
        auto sizePtr = reinterpret_cast<unsigned *>(
            glMapBufferRange(_lightsSSBO->type, offset, sizeof(unsigned), GL_MAP_WRITE_BIT));
        *sizePtr = _dirLights.size();
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
        *sizePtr = _pointLights.size();
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
        *sizePtr = _spotLights.size();
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

} // namespace RenderIt
