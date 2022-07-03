#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace RenderIt
{

inline std::ostream &operator<<(std::ostream &os, const glm::vec2 &v)
{
    os << '<' << v.x << ',' << v.y << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec3 &v)
{
    os << '<' << v.x << ',' << v.y << ',' << v.z << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::vec4 &v)
{
    os << '<' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const glm::quat &q)
{
    os << '<' << q.x << ',' << q.y << ',' << q.z << ',' << q.w << '>';
    return os;
}

inline std::ostream &operator<<(std::ostream& os, const glm::mat3& m)
{
    os << "[\n" << m[0][0] << "," << m[1][0] << "," << m[2][0] << "\n";
    os << m[0][1] << "," << m[1][1] << "," << m[2][1] << "\n";
    os << m[0][2] << "," << m[1][2] << "," << m[2][2] << "\n]";
    return os;
}

inline std::ostream &operator<<(std::ostream& os, const glm::mat4& m)
{
    os << "[\n" << m[0][0] << "," << m[1][0] << "," << m[2][0] << "," << m[3][0] << "\n";
    os << m[0][1] << "," << m[1][1] << "," << m[2][1] << "," << m[3][1] << "\n";
    os << m[0][2] << "," << m[1][2] << "," << m[2][2] << "," << m[3][2] << "\n";
    os << m[0][3] << "," << m[1][3] << "," << m[2][3] << "," << m[3][3] << "\n]";
    return os;
}

} // namespace RenderIt
