#pragma once
#include <string>

/** @file */

namespace RenderIt
{

/// Simple mesh shapes
enum class MeshShape
{
    None,
    Plane,
    Cube,
    Sphere,
    Cylinder,
    Cone,
    Torus
};

extern const std::string MeshShapePlaneSource;
extern const std::string MeshShapeCubeSource;
extern const std::string MeshShapeSphereSource;
extern const std::string MeshShapeCylinderSource;
extern const std::string MeshShapeConeSource;
extern const std::string MeshShapeTorusSource;

} // namespace RenderIt

namespace std
{
inline string to_string(const RenderIt::MeshShape &type)
{
    switch (type)
    {
    case RenderIt::MeshShape::None:
        return "None";
    case RenderIt::MeshShape::Plane:
        return "Plane";
    case RenderIt::MeshShape::Cube:
        return "Cube";
    case RenderIt::MeshShape::Sphere:
        return "Sphere";
    case RenderIt::MeshShape::Cylinder:
        return "Cylinder";
    case RenderIt::MeshShape::Cone:
        return "Cone";
    case RenderIt::MeshShape::Torus:
        return "Torus";
    default:
        return "";
    }
}
} // namespace std
