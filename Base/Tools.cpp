#include "Tools.hpp"

#include <GL/glew.h>
#include <termcolor/termcolor.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#include <commdlg.h>
#elif defined(__linux__)
#include <cstdio>
#include <cstring>
#endif

namespace fs = std::filesystem;

namespace RenderIt
{

void Tools::display_message(const std::string &title, const std::string &message, MessageType type)
{
    switch (type)
    {
    case MessageType::INFO:
        std::cout << "I[" << termcolor::green << title << termcolor::reset << "]: " << message << '\n';
        break;
    case MessageType::WARN:
        std::cout << "W[" << termcolor::yellow << title << termcolor::reset << "]: " << message << '\n';
        break;
    case MessageType::ERROR:
        std::cout << "E[" << termcolor::red << termcolor::blink << title << termcolor::reset << "]: " << message
                  << '\n';
        break;
    }
}

std::string Tools::read_file_content(const std::string &path)
{
    if (!fs::is_regular_file(path))
        return "";
    std::ifstream f(path);
    std::stringstream sstr;
    sstr << f.rdbuf();
    return sstr.str();
}

std::string Tools::select_file_in_explorer(const std::string &title)
{
    const char *NAME = "Tools";
    char filepath[1024];
    memset(filepath, sizeof(filepath), 0);
#if defined(WIN32) || defined(_WIN32)
    filepath[0] = '\0';
    OPENFILENAMEA f{};
    f.lStructSize = sizeof(OPENFILENAMEA);
    f.hwndOwner = NULL;
    f.lpstrFile = filepath;
    f.lpstrFilter = "All Files\0*.*\0\0";
    f.lpstrTitle = title.c_str();
    f.nMaxFile = sizeof(filepath);
    f.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    if (!GetOpenFileNameA(&f))
    {
        display_message(NAME, "No file selected", MessageType::WARN);
        return "";
    }
#elif defined(__linux__)
    FILE *f = popen(("zenity --file-selection --title=\"" + title + "\"").c_str(), "r");
    fgets(filepath, sizeof(filepath), f);
    filepath[strlen(filepath) - 1] = 0;
    pclose(f);
#else
    display_message(NAME, "Unsupported platform for file selection", MessageType::WARN);
    filepath[sizeof(filepath) - 1] = 0;
#endif
    return std::string(filepath);
}

void Tools::ensure_path_separators(std::string &path)
{
    char pref = fs::path::preferred_separator;
    switch (pref)
    {
    case '/': {
        std::replace(path.begin(), path.end(), '\\', pref);
        break;
    }
    case '\\': {
        std::replace(path.begin(), path.end(), '/', pref);
        break;
    }
    }
}

void GLAPIENTRY GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam)
{
    const char *NAME = "GL Callback";
    std::stringstream sstr;
    sstr << "\n(";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        sstr << "High";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        sstr << "Low";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        sstr << "Med";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        sstr << "Noti";
        break;
    }
    sstr << ") <";
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        sstr << "API";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sstr << "Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        sstr << "Other";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sstr << "Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sstr << "Third Party";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sstr << "Window System";
        break;
    }
    sstr << "> [";
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        sstr << "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        sstr << "Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        sstr << "Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        sstr << "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        sstr << "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        sstr << "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        sstr << "Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        sstr << "Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        sstr << "Other";
        break;
    }
    sstr << "]\n";
    sstr << message;
    Tools::display_message(NAME, sstr.str(), Tools::MessageType::INFO);
}

void Tools::set_gl_debug(bool enable, bool filterNotifications)
{
    if (enable)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr,
                              static_cast<GLboolean>(filterNotifications));
    }
    else
    {
        glDisable(GL_DEBUG_OUTPUT);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
}

glm::vec2 Tools::convertAssimpVector(const aiVector2D &v)
{
    return glm::vec2(v.x, v.y);
}

glm::vec3 Tools::convertAssimpVector(const aiVector3D &v)
{
    return glm::vec3(v.x, v.y, v.z);
}

glm::vec3 Tools::convertAssimpColor(const aiColor3D &c)
{
    return glm::vec3(c.r, c.g, c.b);
}

glm::vec4 Tools::convertAssimpColor(const aiColor4D &c)
{
    return glm::vec4(c.r, c.g, c.b, c.a);
}

glm::mat3 Tools::convertAssimpMatrix(const aiMatrix3x3 &m)
{
    glm::mat3 mat{0.0f};
    for (unsigned i = 0; i < 3; ++i)
    {
        for (unsigned j = 0; j < 3; ++j)
            mat[i][j] = static_cast<float>(m[j][i]);
    }
    return mat;
}

glm::mat4 Tools::convertAssimpMatrix(const aiMatrix4x4 &m)
{
    glm::mat4 mat{0.0f};
    for (unsigned i = 0; i < 4; ++i)
    {
        for (unsigned j = 0; j < 4; ++j)
            mat[i][j] = static_cast<float>(m[j][i]);
    }
    return mat;
}

glm::quat Tools::convertAssimpQuaternion(const aiQuaternion &q)
{
    return glm::quat(q.w, q.x, q.y, q.z);
}

void Tools::neutralizeTRSMatrixScale(glm::mat4 &mat)
{
    mat[0] = glm::normalize(mat[0]);
    mat[1] = glm::normalize(mat[1]);
    mat[2] = glm::normalize(mat[2]);
}

} // namespace RenderIt
