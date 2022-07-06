#include "Bounds.hpp"
#include "GLMIOStream.hpp"

namespace RenderIt
{

Bounds::Bounds() : max(std::numeric_limits<float>::min()), min(std::numeric_limits<float>::max()), center(0.0f)
{
}

void Bounds::Merge(const Bounds &other)
{
    max = glm::max(max, other.max);
    min = glm::min(min, other.min);
    center = (max + min) * 0.5f;
}

void Bounds::Update(const glm::vec3 &v)
{
    max = glm::max(v, max);
    min = glm::min(v, min);
    center = (max + min) * 0.5f;
}

float Bounds::Diagonal() const
{
    if (!IsValid())
        return 0.0f;
    return glm::length(max - min);
}

bool Bounds::IsValid() const
{
    bool valid = true;
    valid = valid && (max.x > std::numeric_limits<float>::min());
    valid = valid && (max.y > std::numeric_limits<float>::min());
    valid = valid && (max.z > std::numeric_limits<float>::min());

    valid = valid && (min.x < std::numeric_limits<float>::max());
    valid = valid && (min.y < std::numeric_limits<float>::max());
    valid = valid && (min.z < std::numeric_limits<float>::max());

    valid = valid && (max.x >= min.x);
    valid = valid && (max.y >= min.y);
    valid = valid && (max.z >= min.z);
    return valid;
}

void Bounds::Validate()
{
    if (IsValid())
        return;
    // if not valid, set to default values
    max = glm::vec3(0.0f);
    min = glm::vec3(0.0f);
    center = glm::vec3(0.0f);
}

std::ostream &operator<<(std::ostream &os, const Bounds &b)
{
    os << "B(" << b.max << "|" << b.min << "|" << b.center << ")";
    return os;
}

} // namespace RenderIt
