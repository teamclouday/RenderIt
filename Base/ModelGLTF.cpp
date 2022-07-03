#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "Material.hpp"
#include "Model.hpp"
#include "Tools.hpp"
#include "Vertex.hpp"

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include <cassert>
#include <filesystem>
#include <queue>

namespace fs = std::filesystem;

#define MODEL_NAME_DEFAULT "null"

namespace RenderIt
{

// assimp not good at animation loading
// especially for gltf models
// therefore we try tinygltf

bool Model::LoadGLTF(const std::string &path)
{

    return true;
}

} // namespace RenderIt
