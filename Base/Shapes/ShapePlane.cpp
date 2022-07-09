#include "Shapes/MeshShapes.hpp"

namespace RenderIt
{

const std::string MeshShapePlaneSource = R"(
# Blender v3.2.1 OBJ File: ''
# www.blender.org
o Plane
v -1.000000 0.000000 1.000000
v 1.000000 0.000000 1.000000
v -1.000000 0.000000 -1.000000
v 1.000000 0.000000 -1.000000
vt 1.000000 0.000000
vt 0.000000 1.000000
vt 0.000000 0.000000
vt 1.000000 1.000000
vn 0.0000 1.0000 0.0000
s off
f 2/1/1 3/2/1 1/3/1
f 2/1/1 4/4/1 3/2/1
)";

} // namespace RenderIt
