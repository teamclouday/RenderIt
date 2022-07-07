#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "Material.hpp"
#include "Model.hpp"
#include "Tools.hpp"
#include "Vertex.hpp"

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include <algorithm>
#include <filesystem>
#include <queue>
#include <stdexcept>

namespace fs = std::filesystem;

#define MODEL_NAME_DEFAULT "null"

namespace RenderIt
{

// assimp not good at animation loading
// especially for gltf models
// therefore we try tinygltf

bool Model::LoadGLTF(const std::string &path, bool computeDynamicMeshBounds)
{
    throw std::runtime_error("Model::LoadGLTF not implemented!");

    if (_meshes.size())
        Reset();

    // check extension
    auto pathExt = fs::path(path).extension().string();
    std::transform(pathExt.begin(), pathExt.end(), pathExt.begin(), ::tolower);

    auto binary = false;

    if (pathExt.compare(".glb") == 0)
        binary = true;
    else if (pathExt.compare(".gltf") != 0)
        return false; // unsupported format

    std::string loadErr{""}, loadWarn{""};

    tinygltf::TinyGLTF loader;
    tinygltf::Model gltfModel;

    // load model
    bool loadRes = binary ? loader.LoadBinaryFromFile(&gltfModel, &loadErr, &loadWarn, path)
                          : loader.LoadASCIIFromFile(&gltfModel, &loadErr, &loadWarn, path);
    if (!loadErr.empty())
        Tools::display_message(NAME, "tinygltf " + loadErr, Tools::MessageType::WARN);
    if (!loadWarn.empty())
        Tools::display_message(NAME, "tinygltf " + loadWarn, Tools::MessageType::WARN);
    if (!loadRes || !gltfModel.scenes.size())
        return false;

    // get scene
    const auto &scene =
        gltfModel.scenes[(gltfModel.defaultScene >= 0 && gltfModel.defaultScene < gltfModel.scenes.size())
                             ? gltfModel.defaultScene
                             : 0];
    std::queue<int> nodes;
    for (auto nodeIdx : scene.nodes)
        nodes.push(nodeIdx);

    // traverse nodes
    while (!nodes.empty())
    {
        auto nodeIdx = nodes.front();
        nodes.pop();
        const auto &node = gltfModel.nodes[nodeIdx];

        // load mesh
        if (node.mesh >= 0 && node.mesh < gltfModel.meshes.size())
        {
            const auto &mesh = gltfModel.meshes[node.mesh];

            std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
            _meshes.push_back(newMesh);

            std::vector<Vertex> vertices;
            std::vector<unsigned> indices;
            std::shared_ptr<Material> material = std::make_shared<Material>();

            for (const auto &primitive : mesh.primitives)
            {
                // skip if no indices
                if (primitive.indices < 0)
                    continue;
                // skip if no vertex position
                if (!primitive.attributes.count("POSITION"))
                    continue;
                // vertices
                {
                }
            }
        }

        // traverse children
        for (auto childIdx : node.children)
            nodes.push(childIdx);
    }

    return true;
}

} // namespace RenderIt
