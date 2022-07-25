#include "Skybox.hpp"
#include "Tools.hpp"

#include <algorithm>

#include <stb_image.h>

// reference: https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/master/source/shaders/panorama_to_cubemap.frag

namespace RenderIt
{

Skybox::Skybox() : _skybox(nullptr)
{
    loadDrawShaders();
}

bool Skybox::Load(const std::string &panoramaMap)
{
    int skyboxSize = 0;
    // step 1: load map into texture
    auto mapTex = std::make_unique<STexture>(GL_TEXTURE_2D);
    {
        int w, h, n;
        auto imgSource = stbi_loadf(panoramaMap.c_str(), &w, &h, &n, STBI_rgb_alpha);
        if (imgSource)
        {
            mapTex->Bind();
            glTexParameteri(mapTex->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(mapTex->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(mapTex->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(mapTex->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(mapTex->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(mapTex->type, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, imgSource);
            glGenerateMipmap(mapTex->type);
            mapTex->UnBind();
            stbi_image_free(imgSource);
        }
        else
        {
            Tools::display_message(NAME,
                                   "Failed to load " + panoramaMap + " (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
            return false;
        }
        skyboxSize = std::min(w, h);
    }
    // step 2: create cubemap
    _skybox = std::make_unique<STexture>(GL_TEXTURE_CUBE_MAP);
    _skybox->Bind();
    for (auto i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, skyboxSize, skyboxSize, 0, GL_RGBA, GL_FLOAT,
                     nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    _skybox->UnBind();
    // step 3: create fbo
    auto fbo = std::make_unique<SFBO>();
    // step 4: prepare shader
    auto shader = std::make_shared<Shader>();
    const std::string vertSource = R"(
        #version 450 core
        layout(location = 0) out vec2 vertCoords;
        void main()
        {
            float x = float((gl_VertexID & 1) << 2);
            float y = float((gl_VertexID & 2) << 1);
            vertCoords.x = x * 0.5;
            vertCoords.y = y * 0.5;
            gl_Position = vec4(x - 1.0, y - 1.0, 0, 1);
        }
    )";
    const std::string fragSource = R"(
        #version 450 core
        #define MATH_PI 3.1415926535897932384626433832795
        #define MATH_INV_PI (1.0 / MATH_PI)
        layout(location = 0) out vec4 outColor;
        layout(location = 0) in vec2 vertCoords;
        uniform int face;
        uniform sampler2D panorama;
        vec3 uvToXYZ(int face, vec2 uv)
        {
            if (face == 0)
            {
                return vec3(1.0, uv.y, -uv.x);
            }
            else if (face == 1)
            {
                return vec3(-1.0, uv.y, uv.x);
            }
            else if (face == 2)
            {
                return vec3(+uv.x, -1.0, +uv.y);
            }
            else if (face == 3)
            {
                return vec3(+uv.x, 1.0, -uv.y);
            }
            else if (face == 4)
            {
                return vec3(+uv.x, uv.y, 1.0);
            }
            else
            {
                return vec3(-uv.x, +uv.y, -1.0);
            }
        }
        vec2 dirToUV(vec3 dir)
        {
            return vec2(0.50 + 0.50 * atan(dir.z, dir.x) * MATH_INV_PI, 1.0 - acos(dir.y) * MATH_INV_PI);
        }
        vec3 panoramaToCubeMap(int face, vec2 texCoord)
        {
            vec2 texCoordNew = texCoord * 2.0 - 1.0;
            vec3 scan = uvToXYZ(face, texCoordNew);
            vec3 direction = normalize(scan);
            vec2 src = dirToUV(direction);
            return texture(panorama, src).rgb;
        }
        void main()
        {
            outColor = vec4(panoramaToCubeMap(face, vertCoords), 1.0);
        }
    )";
    shader->AddSource(vertSource, GL_VERTEX_SHADER);
    shader->AddSource(fragSource, GL_FRAGMENT_SHADER);
    if (!shader->Compile())
        return false;
    // step 5: record panorama into skybox
    for (auto i = 0; i < 6; ++i)
    {
        fbo->Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _skybox->Get(),
                               0);
        _skybox->Bind();
        glViewport(0, 0, skyboxSize, skyboxSize);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->Bind();
        shader->UniformInt("panorama", 0);
        shader->TextureBinding(mapTex->Get(), 0u);
        shader->UniformInt("face", i);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        shader->UnBind();
        _skybox->UnBind();
        fbo->UnBind();
    }
    _skybox->Bind();
    glGenerateMipmap(_skybox->type);
    _skybox->UnBind();
    return true;
}

bool Skybox::Load(const std::string &posX, const std::string &negX, const std::string &posY, const std::string &negY,
                  const std::string &posZ, const std::string &negZ)
{
    auto setFace = [&](const std::string &path, GLenum target) -> bool {
        int w, h, n;
        auto imgSource = stbi_loadf(path.c_str(), &w, &h, &n, STBI_rgb_alpha);
        if (imgSource)
        {
            glTexImage2D(target, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, imgSource);
            stbi_image_free(imgSource);
            return true;
        }
        else
        {
            Tools::display_message(NAME, "Failed to load " + path + " (" + std::string(stbi_failure_reason()) + ")",
                                   Tools::MessageType::WARN);
            return false;
        }
    };
    auto res = true;
    _skybox = std::make_unique<STexture>(GL_TEXTURE_CUBE_MAP);
    _skybox->Bind();
    res = res && setFace(posX, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    res = res && setFace(negX, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    res = res && setFace(posY, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    res = res && setFace(negY, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    res = res && setFace(posZ, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    res = res && setFace(negZ, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    _skybox->UnBind();
    return res;
}

void Skybox::ClearBackground(const glm::mat4 &mProj, const glm::mat4 &mView) const
{
    if (!_skybox)
    {
        Tools::display_message(NAME, "no skybox set!", Tools::MessageType::WARN);
        return;
    }
    auto hasDepth = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    auto mProjView = mProj * glm::mat4(glm::mat3(mView));
    _drawShader->Bind();
    _drawShader->UniformMat4("mProjView", mProjView);
    _drawShader->UniformInt("skybox", 0);
    _drawShader->TextureBinding(_skybox->Get(), 0u);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    _drawShader->UnBind();
    if (hasDepth)
        glEnable(GL_DEPTH_TEST);
}

GLuint Skybox::GetSkybox() const
{
    return _skybox ? _skybox->Get() : 0u;
}

void Skybox::loadDrawShaders()
{
    const std::string vertSource = R"(
        #version 450 core
        layout(location = 0) out vec3 vertDir;
        const vec3 skyboxVertices[36] = vec3[36](
            vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, 1.0, -1.0),
            vec3(-1.0, 1.0, -1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, -1.0, -1.0), vec3(-1.0, 1.0, -1.0), vec3(-1.0, 1.0, -1.0),
            vec3(-1.0, 1.0, 1.0), vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, -1.0, 1.0), vec3(1.0, 1.0, 1.0),
            vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, 1.0, 1.0),
            vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, -1.0, 1.0), vec3(-1.0, -1.0, 1.0), vec3(-1.0, 1.0, -1.0),
            vec3(1.0, 1.0, -1.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(-1.0, 1.0, 1.0), vec3(-1.0, 1.0, -1.0),
            vec3(-1.0, -1.0, -1.0), vec3(-1.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0), vec3(1.0, -1.0, -1.0), vec3(-1.0, -1.0, 1.0),
            vec3(1.0, -1.0, 1.0));
        uniform mat4 mProjView;
        void main()
        {
            vertDir = skyboxVertices[gl_VertexID];
            gl_Position = (mProjView * vec4(vertDir, 1.0)).xyww;
        }
    )";
    const std::string fragSource = R"(
        #version 450 core
        layout(location = 0) out vec4 outColor;
        layout(location = 0) in vec3 vertDir;
        uniform samplerCube skybox;
        void main()
        {
            outColor = texture(skybox, vertDir);
        }
    )";
    _drawShader = std::make_unique<Shader>();
    _drawShader->AddSource(vertSource, GL_VERTEX_SHADER);
    _drawShader->AddSource(fragSource, GL_FRAGMENT_SHADER);
    _drawShader->Compile();
}

} // namespace RenderIt
