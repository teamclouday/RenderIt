#include "Shadow.hpp"
#include "Tools.hpp"

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

namespace RenderIt
{

ShadowManager::ShadowManager() : _csmNearOffset(50.0f), _csmOffsets(0.0f, 150.0f)
{
    setupCSM();
}

std::shared_ptr<ShadowManager> ShadowManager::Instance()
{
    auto shadow = std::make_shared<ShadowManager>();
    return shadow;
}

void ShadowManager::AttachCamera(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void ShadowManager::AttachLightManager(std::shared_ptr<LightManager> lights)
{
    _lights = lights;
}

void ShadowManager::RecordShadows(std::function<void(const Shader *)> renderFunc)
{
    if (!_lights || !_camera)
    {
        Tools::display_message(NAME, "lights or camera not set!", Tools::MessageType::WARN);
        return;
    }
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glCullFace(GL_FRONT);
    // directional lights
    {
        computeCSMLightMatrices();
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(_csmOffsets.x, _csmOffsets.y);
        _csmFBO->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        _csmShader->Bind();
        _csmSSBO->BindBase(1u);
        for (auto lightIdx = 0u; lightIdx < _lights->_dirLights.size(); ++lightIdx)
        {
            const auto &light = _lights->_dirLights[lightIdx];
            if (!light.castShadow)
                continue;
            _csmShader->UniformInt("lightIdx", static_cast<int>(lightIdx));
            renderFunc(_csmShader.get());
        }
        _csmSSBO->UnBindBase(1u);
        _csmShader->UnBind();
        _csmFBO->UnBind();
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    glCullFace(GL_BACK);
}

GLuint ShadowManager::GetShadowMaps(LightType type) const
{
    switch (type)
    {
    case LightType::Directional:
    default: {
        return _csmShadowMaps->Get();
    }
    }
}

void ShadowManager::BindShadowData(LightType type, unsigned binding) const
{
    if (!_lights || !_camera)
    {
        Tools::display_message(NAME, "lights or camera not set!", Tools::MessageType::WARN);
        return;
    }
    switch (type)
    {
    case LightType::Directional:
    default: {
        _camera->_csmDataSSBO->BindBase(binding);
        break;
    }
    }
}

void ShadowManager::BindLightSpaceData(LightType type, unsigned binding) const
{
    if (!_lights || !_camera)
    {
        Tools::display_message(NAME, "lights or camera not set!", Tools::MessageType::WARN);
        return;
    }
    switch (type)
    {
    case LightType::Directional:
    default: {
        _csmSSBO->BindBase(binding);
        break;
    }
    }
}

void ShadowManager::setupCSM()
{
    setupCSMBuffers();
    setupCSMShaders();
}

void ShadowManager::setupCSMBuffers()
{
    // setup csm shadow textures
    _csmShadowMaps = std::make_unique<STexture>(GL_TEXTURE_2D_ARRAY);
    _csmShadowMaps->Bind();
    glTexImage3D(_csmShadowMaps->type, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE,
                 SHADOW_CSM_COUNT * LIGHTS_MAX_DIR_LIGHTS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(_csmShadowMaps->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(_csmShadowMaps->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(_csmShadowMaps->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(_csmShadowMaps->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    _csmShadowMaps->UnBind();
    // setup framebuffer
    _csmFBO = std::make_unique<SFBO>();
    _csmFBO->Bind();
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _csmShadowMaps->Get(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (!_csmFBO->Validate())
        Tools::display_message(NAME, "failed to create framebuffer for CSM!", Tools::MessageType::WARN);
    _csmFBO->UnBind();
    // setup SSBO
    _csmSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _csmSSBO->Bind();
    glBufferData(_csmSSBO->type, SHADOW_CSM_COUNT * LIGHTS_MAX_DIR_LIGHTS * sizeof(glm::mat4), nullptr,
                 GL_DYNAMIC_DRAW);
    _csmSSBO->UnBind();
}

void ShadowManager::setupCSMShaders()
{
    std::string vertShader = R"(
        #version 450 core
        layout (location = 0) in vec3 inPos;
        layout(location = 5) in uvec4 inBoneIDs;
        layout(location = 6) in vec4 inBoneWeights;
        uniform mat4 mat_Model;
        layout(std430, binding = 0) readonly buffer BoneMatrices
        {
            mat4 boneMats[];
        };
        void main()
        {
            mat4 boneTransform;
            if ((inBoneWeights[0] + inBoneWeights[1] + inBoneWeights[2] + inBoneWeights[3]) != 0.0)
            {
                boneTransform = boneMats[inBoneIDs[0]] * inBoneWeights[0];
                boneTransform += boneMats[inBoneIDs[1]] * inBoneWeights[1];
                boneTransform += boneMats[inBoneIDs[2]] * inBoneWeights[2];
                boneTransform += boneMats[inBoneIDs[3]] * inBoneWeights[3];
            }
            else
            {
                boneTransform = mat4(1.0);
            }
            gl_Position = mat_Model * boneTransform * vec4(inPos, 1.0);
        }
    )";
    std::string geomShader = R"(
        #version 450 core
        #define LIGHTS_MAX_DIR_LIGHTS 4
        #define SHADOW_CSM_COUNT 4
        layout(triangles, invocations = SHADOW_CSM_COUNT) in;
        layout(triangle_strip, max_vertices = 3) out;
        layout(std430, binding = 1) readonly buffer LightSpaceMats
        {
            mat4 lights[SHADOW_CSM_COUNT * LIGHTS_MAX_DIR_LIGHTS];
        };
        uniform int lightIdx;
        void main()
        {
            for (int i = 0; i < 3; i++)
            {
                gl_Layer = gl_InvocationID + lightIdx * SHADOW_CSM_COUNT;
                gl_Position = lights[gl_Layer] * gl_in[i].gl_Position;
                EmitVertex();
            }
            EndPrimitive();
        }
    )";
    _csmShader = std::make_shared<Shader>();
    _csmShader->AddSource(vertShader, GL_VERTEX_SHADER);
    _csmShader->AddSource(geomShader, GL_GEOMETRY_SHADER);
    _csmShader->Compile();
}

void ShadowManager::computeCSMLightMatrices()
{
    const auto &sphereData = _camera->_csmSphereData;
    glm::vec3 vmax, vmin, vext;
    glm::vec4 origin, offset;
    glm::mat4 view, ortho, lightMatrix;
    float texels = SHADOW_SIZE * 0.5f, texelsInv = 1.0f / texels;
    _csmSSBO->Bind();
    auto matPtr = reinterpret_cast<glm::mat4 *>(glMapBuffer(_csmSSBO->type, GL_WRITE_ONLY));
    for (auto lightIdx = 0u; lightIdx < _lights->_dirLights.size(); ++lightIdx)
    {
        const auto &light = _lights->_dirLights[lightIdx];
        if (!light.castShadow)
            continue;
        auto lightDir = glm::normalize(light.dir);
        auto lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
        if (light.dir == lightUp)
            lightUp = glm::vec3(0.00001f, 1.0f, 0.0f);
        for (int cascadeIdx = 0; cascadeIdx < SHADOW_CSM_COUNT; ++cascadeIdx)
        {
            vmax = glm::vec3(sphereData[cascadeIdx].w);
            vmin = -vmax;
            vext = vmax - vmin;
            ortho = glm::ortho(vmin.x, vmax.x, vmin.y, vmax.y, -_csmNearOffset, _csmNearOffset + vext.z);
            auto center = glm::vec3(sphereData[cascadeIdx]);
            view = glm::lookAt(center + lightDir * _csmNearOffset, center, lightUp);
            lightMatrix = ortho * view;
            // align with texels
            origin = lightMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            origin *= texels;
            offset = glm::round(origin) - origin;
            offset *= texelsInv;
            offset.z = offset.w = 0.0f;
            ortho[3] += offset;
            lightMatrix = ortho * view;
            *(matPtr + (cascadeIdx + lightIdx * SHADOW_CSM_COUNT)) = lightMatrix;
        }
    }
    glUnmapBuffer(_csmSSBO->type);
    _csmSSBO->UnBind();
}

void Camera::setupCSMData()
{
    constexpr float lambda = 0.3f, nd = 0.1f, fd = 2.0f, ratio = fd / nd, count = float(SHADOW_CSM_COUNT);
    _csmDistData[0].x = nd;
    float si;
    for (auto i = 1; i < SHADOW_CSM_COUNT; ++i)
    {
        si = i / count;
        _csmDistData[i].x = lambda * (nd * std::pow(ratio, si)) + (1.0f - lambda) * (nd + (fd - nd) * si);
        _csmDistData[i - 1].y = _csmDistData[i].x * 1.005f;
    }
    _csmDistData[SHADOW_CSM_COUNT - 1].y = fd;

    _csmDataSSBO = std::make_unique<SBuffer>(GL_SHADER_STORAGE_BUFFER);
    _csmDataSSBO->Bind();
    glBufferData(_csmDataSSBO->type, SHADOW_CSM_COUNT * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    _csmDataSSBO->UnBind();
}

void Camera::updateCSMData()
{
    float tanHalfFov = std::tan(glm::radians(_fov) * 0.5f);
    glm::vec2 xNearFar, yNearFar;
    glm::vec3 cameraNear, cameraFar;
    _csmDataSSBO->Bind();
    auto ssboPtr = reinterpret_cast<glm::vec4 *>(glMapBuffer(_csmDataSSBO->type, GL_WRITE_ONLY));
    for (auto cascadeIdx = 0; cascadeIdx < SHADOW_CSM_COUNT; ++cascadeIdx)
    {
        yNearFar = _csmDistData[cascadeIdx] * tanHalfFov;
        xNearFar = yNearFar * _aspect;

        cameraNear = _posVec + _frontVec * _csmDistData[cascadeIdx].x;
        cameraFar = _posVec + _frontVec * _csmDistData[cascadeIdx].y;

        _csmFrustumData[cascadeIdx][0] = cameraNear - _upVec * yNearFar.x - _rightVec * xNearFar.x;
        _csmFrustumData[cascadeIdx][1] = cameraNear + _upVec * yNearFar.x - _rightVec * xNearFar.x;
        _csmFrustumData[cascadeIdx][2] = cameraNear + _upVec * yNearFar.x + _rightVec * xNearFar.x;
        _csmFrustumData[cascadeIdx][3] = cameraNear - _upVec * yNearFar.x + _rightVec * xNearFar.x;
        _csmFrustumData[cascadeIdx][4] = cameraFar - _upVec * yNearFar.y - _rightVec * xNearFar.y;
        _csmFrustumData[cascadeIdx][5] = cameraFar + _upVec * yNearFar.y - _rightVec * xNearFar.y;
        _csmFrustumData[cascadeIdx][6] = cameraFar + _upVec * yNearFar.y + _rightVec * xNearFar.y;
        _csmFrustumData[cascadeIdx][7] = cameraFar - _upVec * yNearFar.y + _rightVec * xNearFar.y;

        glm::vec3 center(0.0f);
        for (const auto &v : _csmFrustumData[cascadeIdx])
            center += v;
        center *= 0.125f;

        float radius = 0.0f;
        for (const auto &v : _csmFrustumData[cascadeIdx])
            radius = glm::max(radius, glm::length(v - center));
        radius = glm::ceil(radius * 16.0f) * 0.0625f;

        _csmSphereData[cascadeIdx].x = center.x;
        _csmSphereData[cascadeIdx].y = center.y;
        _csmSphereData[cascadeIdx].z = center.z;
        _csmSphereData[cascadeIdx].w = radius;
        *(ssboPtr + cascadeIdx) = _csmSphereData[cascadeIdx];
    }
    glUnmapBuffer(_csmDataSSBO->type);
    _csmDataSSBO->UnBind();
}

} // namespace RenderIt
