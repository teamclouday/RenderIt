#include "Shader.hpp"
#include "Tools.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <cassert>

namespace RenderIt
{

Shader::Shader() : _compiled(false), _program(0), _shaders(0)
{
}

Shader::~Shader()
{
    Reset();
}

bool Shader::AddSource(const std::string &source, GLenum type)
{
    if (_compiled)
        Reset();

    GLuint shader = glCreateShader(type);
    const char *content = source.c_str();
    glShaderSource(shader, 1, &content, nullptr);
    glCompileShader(shader);

    GLint success{0};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLint infoLen{0};
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen + 1);
        glGetShaderInfoLog(shader, infoLen, nullptr, infoLog.data());

        std::string message = "Failed to compile shader\n" + std::string(infoLog.data());
        message += "\n" + source;
        display_message(NAME, message, MessageType::ERROR);

        return false;
    }

    _shaders.push_back(shader);
    return true;
}

bool Shader::Compile()
{
    if (_compiled)
        return false;
    _program = glCreateProgram();
    for (auto &shader : _shaders)
        glAttachShader(_program, shader);
    glLinkProgram(_program);

    GLint success{0};
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint infoLen{0};
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLen);
        std::vector<GLchar> infoLog(infoLen + 1);
        glGetProgramInfoLog(_program, infoLen, nullptr, infoLog.data());

        std::string message = "Failed to link shaders\n" + std::string(infoLog.data());
        display_message(NAME, message, MessageType::ERROR);
        Reset();

        return false;
    }

    return _compiled = true;
}

bool Shader::IsCompiled()
{
    return _compiled;
}

void Shader::Bind()
{
    if (_compiled)
        glUseProgram(_program);
}

void Shader::UnBind()
{
    glUseProgram(0);
}

void Shader::Reset()
{
    for (auto &shader : _shaders)
        glDeleteShader(shader);
    if (_compiled)
        glDeleteProgram(_program);
    _shaders.resize(0);
    _compiled = false;
}

GLuint Shader::GetProgram()
{
    return _program;
}

void Shader::ConfigMaterialTextures(std::shared_ptr<Material> mat)
{
    if (!_compiled)
        return;

    const std::vector<std::shared_ptr<STexture>> maps = {
        mat->diffuse,   mat->specular,   mat->ambient,      mat->emissive,      mat->height,        mat->normals,
        mat->shininess, mat->opacity,    mat->displacement, mat->lightmap,      mat->reflection,

        mat->pbr_color, mat->pbr_normal, mat->pbr_emission, mat->pbr_metalness, mat->pbr_roughness, mat->pbr_occlusion,
    };
    const std::vector<std::string> mapNames = {
        Material::mapNameDiffuse,      Material::mapNameSpecular,     Material::mapNameAmbient,
        Material::mapNameEmissive,     Material::mapNameHeight,       Material::mapNameNormals,
        Material::mapNameShininess,    Material::mapNameOpacity,      Material::mapNameDisplacement,
        Material::mapNameLightmap,     Material::mapNameReflection,

        Material::mapNamePBRColor,     Material::mapNamePBRNormal,    Material::mapNamePBREmission,
        Material::mapNamePBRMetalness, Material::mapNamePBRRoughness, Material::mapNamePBROcclusion,
    };

    assert(maps.size() == mapNames.size());

    int texIdx = 0;
    for (auto i = 0; i < maps.size(); i++)
    {
        auto &m = maps[i];
        auto &n = mapNames[i];
        if (m)
        {
            glActiveTexture(GL_TEXTURE0 + texIdx);
            m->Bind();
            UniformInt(n, texIdx);
            texIdx++;
            UniformBool(n + Material::existsEXT, true);
        }
        else
            UniformBool(n + Material::existsEXT, false);
    }
}

void Shader::UniformBool(const std::string &name, bool val) const
{
    if (!_compiled)
        return;
    glUniform1i(glGetUniformLocation(_program, name.c_str()), static_cast<int>(val));
}

void Shader::UniformInt(const std::string &name, int val) const
{
    if (!_compiled)
        return;
    glUniform1i(glGetUniformLocation(_program, name.c_str()), val);
}

void Shader::UniformFloat(const std::string &name, float val) const
{
    if (!_compiled)
        return;
    glUniform1f(glGetUniformLocation(_program, name.c_str()), val);
}

void Shader::UniformVec2(const std::string &name, const glm::vec2 &val) const
{
    if (!_compiled)
        return;
    glUniform2fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformVec3(const std::string &name, const glm::vec3 &val) const
{
    if (!_compiled)
        return;
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformVec4(const std::string &name, const glm::vec4 &val) const
{
    if (!_compiled)
        return;
    glUniform4fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformIVec2(const std::string &name, const glm::ivec2 &val) const
{
    if (!_compiled)
        return;
    glUniform2iv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformIVec3(const std::string &name, const glm::ivec3 &val) const
{
    if (!_compiled)
        return;
    glUniform3iv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformIVec4(const std::string &name, const glm::ivec4 &val) const
{
    if (!_compiled)
        return;
    glUniform4iv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::UniformMat2(const std::string &name, const glm::mat2 &val) const
{
    if (!_compiled)
        return;
    glUniformMatrix2fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::UniformMat3(const std::string &name, const glm::mat3 &val) const
{
    if (!_compiled)
        return;
    glUniformMatrix3fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::UniformMat4(const std::string &name, const glm::mat4 &val) const
{
    if (!_compiled)
        return;
    glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::UboBinding(const std::string &name, uint32_t binding)
{
    if (!_compiled)
        return;
    GLuint idx = glGetUniformBlockIndex(_program, name.c_str());
    glUniformBlockBinding(_program, idx, binding);
}

} // namespace RenderIt
