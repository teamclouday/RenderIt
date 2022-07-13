#include "Shader.hpp"
#include "Tools.hpp"

#include <glm/gtc/type_ptr.hpp>

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
        Tools::display_message(LOGNAME, message, Tools::MessageType::ERROR);

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
        Tools::display_message(LOGNAME, message, Tools::MessageType::ERROR);
        Reset();

        return false;
    }

    return _compiled = true;
}

bool Shader::IsCompiled() const
{
    return _compiled;
}

void Shader::Bind() const
{
    if (_compiled)
        glUseProgram(_program);
}

void Shader::UnBind() const
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

GLuint Shader::GetProgram() const
{
    return _program;
}

void Shader::ConfigMaterialTextures(const Material *mat) const
{
    if (!_compiled)
        return;

    // bind textures
    int texIdx = 0;
    auto bindTexture = [&](const std::shared_ptr<STexture> &tex, const std::string &name) {
        if (tex)
        {
            TextureBinding(tex->Get(), static_cast<uint32_t>(texIdx));
            UniformInt(name, texIdx);
            UniformBool(name + Material::existsEXT, true);
            ++texIdx;
        }
        else
            UniformBool(name + Material::existsEXT, false);
    };

    bindTexture(mat->diffuse, Material::mapNameDiffuse);
    bindTexture(mat->specular, Material::mapNameSpecular);
    bindTexture(mat->ambient, Material::mapNameAmbient);
    bindTexture(mat->emissive, Material::mapNameEmissive);
    bindTexture(mat->height, Material::mapNameHeight);
    bindTexture(mat->normals, Material::mapNameNormals);
    bindTexture(mat->shininess, Material::mapNameShininess);
    bindTexture(mat->opacity, Material::mapNameOpacity);
    bindTexture(mat->displacement, Material::mapNameDisplacement);
    bindTexture(mat->lightmap, Material::mapNameLightmap);
    bindTexture(mat->reflection, Material::mapNameReflection);

    bindTexture(mat->pbr_color, Material::mapNamePBRColor);
    bindTexture(mat->pbr_normal, Material::mapNamePBRNormal);
    bindTexture(mat->pbr_emission, Material::mapNamePBREmission);
    bindTexture(mat->pbr_metalness, Material::mapNamePBRMetalness);
    bindTexture(mat->pbr_roughness, Material::mapNamePBRRoughness);
    bindTexture(mat->pbr_occlusion, Material::mapNamePBROcclusion);

    // set constants
    UniformVec3(Material::valNameColorAmbient, mat->colorAmbient);
    UniformVec3(Material::valNameColorDiffuse, mat->colorDiffuse);
    UniformVec3(Material::valNameColorSpecular, mat->colorSpecular);
    UniformVec3(Material::valNameColorEmissive, mat->colorEmissive);
    UniformFloat(Material::valNameColorShininess, mat->valShininess);
    UniformFloat(Material::valNameColorOpacity, mat->valOpacity);
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

void Shader::UboBinding(const std::string &name, uint32_t binding) const
{
    if (!_compiled)
        return;
    GLuint idx = glGetUniformBlockIndex(_program, name.c_str());
    glUniformBlockBinding(_program, idx, binding);
}

void Shader::SsboBinding(const std::string &name, uint32_t binding) const
{
    if (!_compiled)
        return;
    GLuint idx = glGetProgramResourceIndex(_program, GL_SHADER_STORAGE_BLOCK, name.c_str());
    glShaderStorageBlockBinding(_program, idx, binding);
}

void Shader::TextureBinding(const GLuint &texID, uint32_t binding) const
{
    if (!_compiled)
        return;
    glBindTextureUnit(binding, texID);
}

} // namespace RenderIt
