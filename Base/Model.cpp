#define STB_IMAGE_IMPLEMENTATION

#include "Model.hpp"
#include "Tools.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <filesystem>
#include <queue>

namespace fs = std::filesystem;

namespace RenderIt
{

Model::Model()
{
}

Model::~Model()
{
    Reset();
}

bool Model::Load(const std::string &path, unsigned flags)
{
    if (_meshes.size())
        Reset();

    auto directory = fs::path(path).parent_path();

    // load file
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, flags);
    if (!scene)
        return false;

    // process nodes
    std::queue<aiNode *> nodes;
    nodes.push(scene->mRootNode);
    while (!nodes.empty())
    {
        auto node = nodes.front();
        nodes.pop();
        // create meshes
        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            auto mesh = scene->mMeshes[node->mMeshes[i]];

            std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
            _meshes.push_back(newMesh);

            std::vector<Vertex> vertices;
            std::vector<unsigned> indices;
            std::shared_ptr<Material> material = std::make_shared<Material>();

            // vertex data
            for (auto i = 0; i < mesh->mNumVertices; i++)
            {
                auto position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                auto normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                auto texcoords = mesh->mTextureCoords[0]
                                     ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                                     : glm::vec2(0.0f);
                auto color = mesh->mColors[0] ? glm::vec4(mesh->mColors[0][i].r, mesh->mColors[0][i].g,
                                                          mesh->mColors[0][i].b, mesh->mColors[0][i].a)
                                              : glm::vec4(1.0f);
                vertices.push_back({position, normal, texcoords, color});
            }

            // indices data
            for (auto i = 0; i < mesh->mNumFaces; i++)
            {
                auto face = mesh->mFaces[i];
                for (auto j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            // material data
            auto mat = scene->mMaterials[mesh->mMaterialIndex];
            aiString texturePath;

            // legacy materials
            if (mat->GetTextureCount(aiTextureType_DIFFUSE))
            {
                mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->diffuse = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_SPECULAR))
            {
                mat->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->specular = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_AMBIENT))
            {
                mat->GetTexture(aiTextureType_AMBIENT, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->ambient = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_EMISSIVE))
            {
                mat->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->emissive = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_HEIGHT))
            {
                mat->GetTexture(aiTextureType_HEIGHT, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->height = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_NORMALS))
            {
                mat->GetTexture(aiTextureType_NORMALS, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->normals = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_SHININESS))
            {
                mat->GetTexture(aiTextureType_SHININESS, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->shininess = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_OPACITY))
            {
                mat->GetTexture(aiTextureType_OPACITY, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->opacity = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_DISPLACEMENT))
            {
                mat->GetTexture(aiTextureType_DISPLACEMENT, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->displacement = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_LIGHTMAP))
            {
                mat->GetTexture(aiTextureType_LIGHTMAP, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->lightmap = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_REFLECTION))
            {
                mat->GetTexture(aiTextureType_REFLECTION, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->reflection = LoadTexture(path);
            }

            // pbr materials
            if (mat->GetTextureCount(aiTextureType_BASE_COLOR))
            {
                mat->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_color = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_NORMAL_CAMERA))
            {
                mat->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_normal = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_EMISSION_COLOR))
            {
                mat->GetTexture(aiTextureType_EMISSION_COLOR, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_emission = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_METALNESS))
            {
                mat->GetTexture(aiTextureType_METALNESS, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_metalness = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS))
            {
                mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_roughness = LoadTexture(path);
            }
            if (mat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION))
            {
                mat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath);
                auto path = directory / fs::path(texturePath.C_Str());
                material->pbr_occlusion = LoadTexture(path);
            }

            newMesh->Load(vertices, indices, material, GL_TRIANGLES);
        }
        // add sub nodes
        for (auto i = 0; i < node->mNumChildren; i++)
            nodes.push(node->mChildren[i]);
    }

    return true;
}

bool Model::Load(MeshShape shape)
{
    if (_meshes.size())
        Reset();
    std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
    _meshes.push_back(newMesh);
    newMesh->Load(shape);
}

void Model::Draw(Shader *shader)
{
    for (auto &mesh : _meshes)
        mesh->Draw(shader);
}

void Model::Reset()
{
    _meshes.clear();
    _meshes.resize(0);
}

std::shared_ptr<STexture> Model::LoadTexture(const std::string &path)
{
    if (_textures.count(path))
        return _textures[path];

    int w{0}, h{0}, n{4};
    unsigned char *data = stbi_load(path.c_str(), &w, &h, &n, STBI_rgb_alpha);
    if (!data)
    {
        display_message(NAME, "Failed to load model file " + path + "\n" + std::string(stbi_failure_reason()),
                        MessageType::WARN);
        return nullptr;
    }

    std::shared_ptr<STexture> tex = std::make_shared<STexture>(GL_TEXTURE_2D);
    tex->Bind();
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(tex->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(tex->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    tex->UnBind();

    stbi_image_free(data);
    return _textures[path] = tex;
}

} // namespace RenderIt
