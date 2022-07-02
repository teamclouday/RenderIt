#pragma once

#include <iostream>

#include <glm/glm.hpp>

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

} // namespace RenderIt
