#pragma once

#include <iostream>
#include <string>

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

/** @file */

namespace RenderIt
{

/// Collection of tools
struct Tools
{
    /// Defines display message types
    enum class MessageType
    {
        INFO = 0,
        WARN = 1,
        ERROR = 2
    };

    /// Prints to terminal
    template <typename... T> static void println(T &&...args)
    {
        (std::cout << ... << args) << '\n';
    }

    /// Display message in terminal with type
    static void display_message(const std::string &title, const std::string &message, MessageType type);

    /// Read full file content into string
    static std::string read_file_content(const std::string &path);

    /// Select file from explorer
    static std::string select_file_in_explorer(const std::string &title = "Select File");

    /// Ensure separators are valid
    static void ensure_path_separators(std::string &path);

    /// Enable/Disable OpenGL debug output
    static void set_gl_debug(bool enable, bool filterNotifications = true);

#pragma region assimp_conversions

    /// Convert aiVector2D to vec2
    static glm::vec2 convertAssimpVector(const aiVector2D &v);

    /// Convert aiVector3D to vec3
    static glm::vec3 convertAssimpVector(const aiVector3D &v);

    /// Convert aiColor3D to vec3
    static glm::vec3 convertAssimpColor(const aiColor3D &c);

    /// Convert aiColor4D to vec4
    static glm::vec4 convertAssimpColor(const aiColor4D &c);

    /// Convert aiMatrix3x3 to mat3
    static glm::mat3 convertAssimpMatrix(const aiMatrix3x3 &m);

    /// Convert aiMatrix4x4 to mat4
    static glm::mat4 convertAssimpMatrix(const aiMatrix4x4 &m);

    /// Convert aiQuaternion to quat
    static glm::quat convertAssimpQuaternion(const aiQuaternion &q);

#pragma endregion assimp_conversions

    /// Neutralize transformation matrix scale (assuming TRS)
    static void neutralizeTRSMatrixScale(glm::mat4 &mat);

    /// Multiply a matrix with a positional vector
    static glm::vec3 matrixMultiplyPoint(const glm::mat4 &mat, const glm::vec3 &v);

    /// Multiply a matrix with a directional vector
    static glm::vec3 matrixMultiplyVector(const glm::mat4 &mat, const glm::vec3 &v);
};

} // namespace RenderIt
