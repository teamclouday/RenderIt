#define STB_IMAGE_IMPLEMENTATION

#include "Model.hpp"
#include "Tools.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <cassert>
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
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
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
                auto texcoords = mesh->HasTextureCoords(0)
                                     ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                                     : glm::vec2(0.0f);
                auto tangent = mesh->HasTangentsAndBitangents()
                                   ? glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z)
                                   : glm::vec3(0.0f);
                auto bitangent = mesh->HasTangentsAndBitangents()
                                     ? glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z)
                                     : glm::vec3(0.0f);
                vertices.push_back({position, normal, texcoords, tangent, bitangent});

                // update bounds
                bounds.Update(position);
            }

            // indices data
            for (auto i = 0; i < mesh->mNumFaces; i++)
            {
                auto face = mesh->mFaces[i];
                assert(face.mNumIndices == 3);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }

            // material data
            auto mat = scene->mMaterials[mesh->mMaterialIndex];
            aiString texturePath;

            auto setTexture = [&](aiTextureType type, std::shared_ptr<STexture> &target) {
                if (mat->GetTextureCount(type))
                {
                    mat->GetTexture(type, 0, &texturePath);
                    if (auto tex = scene->GetEmbeddedTexture(texturePath.C_Str()))
                    {
                        unsigned char *data = reinterpret_cast<unsigned char *>(tex->pcData);
                        target = LoadTexture(data, tex->mWidth, tex->mHeight, texturePath.C_Str());
                    }
                    else
                    {
                        auto path = directory / fs::path(texturePath.C_Str());
                        target = LoadTexture(path);
                    }
                }
            };

            // legacy materials
            setTexture(aiTextureType_DIFFUSE, material->diffuse);
            setTexture(aiTextureType_SPECULAR, material->specular);
            setTexture(aiTextureType_AMBIENT, material->ambient);
            setTexture(aiTextureType_EMISSIVE, material->emissive);
            setTexture(aiTextureType_HEIGHT, material->height);
            setTexture(aiTextureType_NORMALS, material->normals);
            setTexture(aiTextureType_SHININESS, material->shininess);
            setTexture(aiTextureType_OPACITY, material->opacity);
            setTexture(aiTextureType_DISPLACEMENT, material->displacement);
            setTexture(aiTextureType_LIGHTMAP, material->lightmap);
            setTexture(aiTextureType_REFLECTION, material->reflection);

            // pbr materials
            setTexture(aiTextureType_BASE_COLOR, material->pbr_color);
            setTexture(aiTextureType_NORMAL_CAMERA, material->pbr_normal);
            setTexture(aiTextureType_EMISSION_COLOR, material->pbr_emission);
            setTexture(aiTextureType_METALNESS, material->pbr_metalness);
            setTexture(aiTextureType_DIFFUSE_ROUGHNESS, material->pbr_roughness);
            setTexture(aiTextureType_AMBIENT_OCCLUSION, material->pbr_occlusion);

            // material constants
            aiColor3D color;
            float val{0.0f};
            if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
                material->colorAmbient = glm::vec3(color.r, color.g, color.b);
            if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                material->colorDiffuse = glm::vec3(color.r, color.g, color.b);
            if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
                material->colorSpecular = glm::vec3(color.r, color.g, color.b);
            if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
                material->colorEmissive = glm::vec3(color.r, color.g, color.b);

            if (mat->Get(AI_MATKEY_SHININESS, val) == AI_SUCCESS)
                material->valShininess = val;
            if (mat->Get(AI_MATKEY_OPACITY, val) == AI_SUCCESS)
                material->valOpacity = val;

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
    return true;
}

void Model::Draw(std::shared_ptr<Shader> shader) const
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
    auto p = path;
    Tools::ensure_path_separators(p);

    if (_textures.count(p))
        return _textures[p];

    int w{0}, h{0}, n{4};
    unsigned char *data = stbi_load(p.c_str(), &w, &h, &n, STBI_rgb_alpha);
    if (!data)
    {
        Tools::display_message(NAME, "Failed to load texture file " + p + "\n" + std::string(stbi_failure_reason()),
                               Tools::MessageType::WARN);
        return nullptr;
    }

    auto tex = std::make_shared<STexture>(GL_TEXTURE_2D);
    tex->Bind();
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(tex->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    tex->UnBind();

    stbi_image_free(data);
    return _textures[p] = tex;
}

std::shared_ptr<STexture> Model::LoadTexture(unsigned char *pixels, int width, int height, const std::string &name)
{
    if (_textures.count(name))
        return _textures[name];

    int w{0}, h{0}, n{4};
    unsigned char *data;
    if (!height)
        data = stbi_load_from_memory(pixels, width, &w, &h, &n, STBI_rgb_alpha);
    else
        data = stbi_load_from_memory(pixels, width * height, &w, &h, &n, STBI_rgb_alpha);
    if (!data)
    {
        Tools::display_message(NAME,
                               "Failed to load embedded texture " + name + "\n" + std::string(stbi_failure_reason()),
                               Tools::MessageType::WARN);
        return nullptr;
    }

    auto tex = std::make_shared<STexture>(GL_TEXTURE_2D);
    tex->Bind();
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(tex->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(tex->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    tex->UnBind();

    stbi_image_free(data);
    return _textures[name] = tex;
}

} // namespace RenderIt
