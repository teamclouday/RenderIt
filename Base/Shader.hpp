#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

#include "GLStructs.hpp"

/** @file */

namespace RenderIt
{

/// Shader program helper class
class Shader
{
  public:
    Shader();

    ~Shader();

    /// Add shader source to compile
    bool AddSource(const std::string &source, GLenum type);

    /// Link the added shaders and compile a shader program
    bool Compile();

    /// Is program compiled
    bool IsCompiled();

    /// Bind current program
    void Bind();

    /// Unbind current program
    void UnBind();

    /// Reset program
    void Reset();

    /// Get raw program
    GLuint GetProgram();

    /// Configure material textures
    void ConfigMaterialTextures(std::shared_ptr<Material> mat);

#pragma region uniform_methods

    void UniformBool(const std::string &name, bool val) const;

    void UniformInt(const std::string &name, int val) const;

    void UniformFloat(const std::string &name, float val) const;

    void UniformVec2(const std::string &name, const glm::vec2 &val) const;

    void UniformVec3(const std::string &name, const glm::vec3 &val) const;

    void UniformVec4(const std::string &name, const glm::vec4 &val) const;

    void UniformIVec2(const std::string &name, const glm::ivec2 &val) const;

    void UniformIVec3(const std::string &name, const glm::ivec3 &val) const;

    void UniformIVec4(const std::string &name, const glm::ivec4 &val) const;

    void UniformMat2(const std::string &name, const glm::mat2 &val) const;

    void UniformMat3(const std::string &name, const glm::mat3 &val) const;

    void UniformMat4(const std::string &name, const glm::mat4 &val) const;

    void UboBinding(const std::string &name, uint32_t binding);

#pragma endregion uniform_methods

  public:
    const std::string NAME = "Shader";

  private:
    bool _compiled;
    GLuint _program;
    std::vector<GLuint> _shaders;
};

} // namespace RenderIt
