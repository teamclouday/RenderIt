#include "Model.hpp"
#include "Animator.hpp"
#include "Material.hpp"
#include "Tools.hpp"
#include "Vertex.hpp"

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <cassert>
#include <filesystem>
#include <queue>
#include <tuple>

namespace fs = std::filesystem;

#define MODEL_NAME_DEFAULT "null"

namespace RenderIt
{

Model::Model()
    : modelName(MODEL_NAME_DEFAULT), transform(Transform::Type::TRS), _animationActive(0), _animNodeRoot(nullptr),
      _parent(nullptr)
{
}

Model::~Model()
{
    Reset();
}

bool Model::Load(const std::string &path, unsigned flags, bool computeDynamicMeshBounds)
{
    if (_meshes.size())
        Reset();

    auto directory = fs::path(path).parent_path();

    // load file
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, flags);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return false;

    auto sceneName = std::string(scene->mName.C_Str());
    modelName = sceneName.length() > 0 ? sceneName : fs::path(path).filename().string();

    // prepare animations
    loadAnimationTree(scene);
    updateAnimations(scene);

    // process nodes
    std::queue<std::tuple<aiNode *, int, glm::mat4, glm::mat4>> nodes;
    nodes.push({scene->mRootNode, -1, glm::mat4(1.0f), glm::mat4(1.0f)});
    while (!nodes.empty())
    {
        auto data = nodes.front();
        // current node
        auto node = std::get<0>(data);
        // parent node that has bone ID, but without mesh, -1 if invalid
        auto nodeParentBoneID = std::get<1>(data);
        // relative transform from nodeParentBoneID
        auto nodeParentBoneT = std::get<2>(data);
        // global transform
        auto nodeGlobalT = std::get<3>(data);
        nodes.pop();

        auto nodeName = std::string(node->mName.C_Str());
        auto nodeHasMesh = node->mNumMeshes > 0;
        auto nodeHasBoneInTree = _boneInfo.count(nodeName) > 0;
        auto nodeCurrT = Tools::convertAssimpMatrix(node->mTransformation);

        nodeGlobalT = nodeGlobalT * nodeCurrT;
        nodeParentBoneT = nodeParentBoneT * nodeCurrT;

        // create meshes
        for (auto meshIdx = 0u; meshIdx < node->mNumMeshes; ++meshIdx)
        {
            auto mesh = scene->mMeshes[node->mMeshes[meshIdx]];

            std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
            _meshes.push_back(newMesh);

            std::vector<Vertex> vertices;
            std::vector<unsigned> indices;
            std::shared_ptr<Material> material = std::make_shared<Material>();

            // vertex data
            for (auto vertexIdx = 0u; vertexIdx < mesh->mNumVertices; ++vertexIdx)
            {
                auto position = Tools::convertAssimpVector(mesh->mVertices[vertexIdx]);
                auto normal = Tools::convertAssimpVector(mesh->mNormals[vertexIdx]);
                auto texcoords = mesh->HasTextureCoords(0)
                                     ? glm::vec2(Tools::convertAssimpVector(mesh->mTextureCoords[0][vertexIdx]))
                                     : glm::vec2(0.0f);
                auto tangent = mesh->HasTangentsAndBitangents() ? Tools::convertAssimpVector(mesh->mTangents[vertexIdx])
                                                                : glm::vec3(0.0f);
                auto bitangent = mesh->HasTangentsAndBitangents()
                                     ? Tools::convertAssimpVector(mesh->mBitangents[vertexIdx])
                                     : glm::vec3(0.0f);
                auto defaultBoneID = glm::uvec4(0);
                auto defaultBoneWeights = glm::vec4(0.0f);

                // transform mesh if no bone attached
                if (!mesh->mNumBones)
                {
                    // if parent bone exist, use relative transform
                    if (nodeParentBoneID >= 0)
                    {
                        position = Tools::matrixMultiplyPoint(nodeParentBoneT, position);
                        normal = Tools::matrixMultiplyVector(nodeParentBoneT, normal);
                        tangent = Tools::matrixMultiplyVector(nodeParentBoneT, tangent);
                        bitangent = Tools::matrixMultiplyVector(nodeParentBoneT, bitangent);
                    }
                    else
                    {
                        position = Tools::matrixMultiplyPoint(nodeGlobalT, position);
                        normal = Tools::matrixMultiplyVector(nodeGlobalT, normal);
                        tangent = Tools::matrixMultiplyVector(nodeGlobalT, tangent);
                        bitangent = Tools::matrixMultiplyVector(nodeGlobalT, bitangent);
                        // update bounds for static meshes
                        bounds.Update(position);
                    }
                }

                vertices.push_back(
                    {position, normal, texcoords, tangent, bitangent, defaultBoneID, defaultBoneWeights});
            }

            // indices data
            for (auto faceIdx = 0u; faceIdx < mesh->mNumFaces; ++faceIdx)
            {
                auto face = mesh->mFaces[faceIdx];
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
                        target = LoadTexture(path.string());
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
            float fval{0.0f};
            int ival{0};
            if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
                material->colorAmbient = Tools::convertAssimpColor(color);
            if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
                material->colorDiffuse = Tools::convertAssimpColor(color);
            if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
                material->colorSpecular = Tools::convertAssimpColor(color);
            if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
                material->colorEmissive = Tools::convertAssimpColor(color);

            if (mat->Get(AI_MATKEY_SHININESS, fval) == AI_SUCCESS)
                material->valShininess = fval;
            if (mat->Get(AI_MATKEY_OPACITY, fval) == AI_SUCCESS)
                material->valOpacity = fval;

            if (mat->Get(AI_MATKEY_TWOSIDED, ival) == AI_SUCCESS)
                material->twoSided = ival != 0;

            // vertex bone info
            for (auto boneIdx = 0u; boneIdx < mesh->mNumBones; ++boneIdx)
            {
                unsigned boneID;
                std::string boneName = mesh->mBones[boneIdx]->mName.C_Str();
                if (!_boneInfo.count(boneName))
                {
                    boneID = static_cast<unsigned>(_boneInfo.size());
                    _boneInfo[boneName] = {boneID, Tools::convertAssimpMatrix(mesh->mBones[boneIdx]->mOffsetMatrix)};
                }
                else
                {
                    boneID = _boneInfo[boneName].first;
                    if (!_boneInfo[boneName].second.has_value())
                        _boneInfo[boneName].second = Tools::convertAssimpMatrix(mesh->mBones[boneIdx]->mOffsetMatrix);
                }
                assert(boneID >= 0);

                auto weights = mesh->mBones[boneIdx]->mWeights;
                auto numWeights = mesh->mBones[boneIdx]->mNumWeights;

                for (auto weightIdx = 0u; weightIdx < numWeights; ++weightIdx)
                {
                    auto vertexId = weights[weightIdx].mVertexId;
                    assert(vertexId <= vertices.size());
                    // set vertex info
                    auto &vertex = vertices[vertexId];
                    for (auto i = 0; i < 4; ++i)
                    {
                        if (!vertex.boneIDs[i] && !vertex.boneWeights[i])
                        {
                            vertex.boneIDs[i] = boneID;
                            vertex.boneWeights[i] = weights[weightIdx].mWeight;
                            break;
                        }
                    }
                }
            }

            // map bone ID if parent (bone, no mesh) & curr node (no bone, mesh)
            if (nodeParentBoneID >= 0 && !mesh->mNumBones)
            {
                auto boneID = static_cast<unsigned>(nodeParentBoneID);
                for (auto &vertex : vertices)
                {
                    vertex.boneIDs[0] = boneID;
                    vertex.boneWeights[0] = 1.0f;
                }
            }

            newMesh->Load(vertices, indices, material, GL_TRIANGLES);
        }

        if (nodeHasBoneInTree)
        {
            if (!nodeHasMesh)
            {
                nodeParentBoneID = static_cast<int>(_boneInfo[nodeName].first);
                nodeParentBoneT = glm::mat4(1.0f);
            }
            else
            {
                nodeParentBoneID = -1;
                nodeParentBoneT = nodeGlobalT;
            }
        }

        // add sub nodes
        for (auto nodeIdx = 0u; nodeIdx < node->mNumChildren; ++nodeIdx)
            nodes.push({node->mChildren[nodeIdx], nodeParentBoneID, nodeParentBoneT, nodeGlobalT});
    }

    if (computeDynamicMeshBounds)
        updateDynamicBounds(scene);

    bounds.Validate();

    return true;
}

bool Model::Load(MeshShape shape)
{
    if (_meshes.size())
        Reset();
    std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();
    _meshes.push_back(newMesh);
    newMesh->Load(shape);
    modelName = std::to_string(shape);
    return true;
}

bool Model::LoadAnimation(const std::string &path)
{
    if (!_meshes.size())
        return false;

    // load file
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, 0);
    if (!scene || !scene->mRootNode || !scene->HasAnimations())
        return false;

    // read animation structure
    loadAnimationTree(scene);
    // load animatins
    updateAnimations(scene);

    return true;
}

void Model::Draw(const Shader *shader) const
{
    for (auto &mesh : _meshes)
        mesh->Draw(shader);
}

void Model::Reset()
{
    _meshes.clear();
    _meshes.resize(0);
    _children.clear();
    _children.resize(0);
    _parent = nullptr;
    modelName = MODEL_NAME_DEFAULT;
    _animations.clear();
    _animations.resize(0);
    _boneInfo.clear();
    _animNodeRoot = nullptr;
}

bool Model::AddChild(std::shared_ptr<Model> child)
{
    // validate
    auto tmp = child;
    while (tmp)
    {
        if (tmp.get() == this)
            return false;
        tmp = tmp->GetParent();
    }
    _children.push_back(child);
    child->_parent = std::shared_ptr<Model>(this);
    return true;
}

std::shared_ptr<Model> Model::GetParent() const
{
    return _parent;
}

std::shared_ptr<Model> Model::GetChild(unsigned idx) const
{
    if (idx >= _children.size())
        return nullptr;
    return _children[idx];
}

size_t Model::GetNumChildren() const
{
    return _children.size();
}

void Model::SetActiveAnimation(unsigned idx)
{
    if (idx < _animations.size())
        _animationActive = idx;
}

size_t Model::GetNumAnimations() const
{
    return _animations.size();
}

bool Model::HasAnimation() const
{
    return _animations.size() > 0;
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

bool Model::updateDynamicBounds(const aiScene *scene)
{
    if (!scene->HasAnimations() || _animations.empty() || _meshes.empty())
        return false;
    // first let animator play full animation
    auto anim = Animator::Instance();
    anim->Update(_animations[_animationActive]->duration);
    anim->UpdateAnimation(this);
    // read access bones
    auto boneMatrices = anim->AccessBoneMatrices();
    // iterate meshes & read transformed data
    for (auto &mesh : _meshes)
    {
        auto VBO = mesh->GetVertexBuffer();
        if (!VBO.has_value())
            continue;
        auto vertexCount = mesh->GetNumVertices();
        // map vertices
        auto dataPtr = reinterpret_cast<Vertex *>(glMapNamedBuffer(VBO.value(), GL_READ_ONLY));
        for (auto vertexIdx = 0; vertexIdx < vertexCount; vertexIdx++, dataPtr++)
        {
            auto pos = (*dataPtr).position;
            auto boneIDs = (*dataPtr).boneIDs;
            auto boneWs = (*dataPtr).boneWeights;
            // check if has bone
            if (boneWs[0] + boneWs[1] + boneWs[2] + boneWs[3])
            {
                auto mat = glm::mat4(0.0f);
                for (auto i = 0; i < 4; i++)
                    mat += boneMatrices[boneIDs[i]] * boneWs[i];
                bounds.Update(Tools::matrixMultiplyPoint(mat, pos));
            }
        }
        glUnmapNamedBuffer(VBO.value());
    }
    anim->Update(0.0f);
    return true;
}

bool Model::updateAnimations(const aiScene *scene)
{
    if (!scene || !scene->HasAnimations())
        return false;
    for (auto animIdx = 0u; animIdx < scene->mNumAnimations; ++animIdx)
        _animations.push_back(std::make_shared<Animation>(scene->mAnimations[animIdx], _boneInfo));
    return true;
}

bool Model::loadAnimationTree(const aiScene *scene)
{
    if (!scene || _animNodeRoot)
        return false;
    std::queue<std::pair<aiNode *, std::shared_ptr<Animation::Node>>> animNodes;
    animNodes.push({scene->mRootNode, nullptr});
    while (!animNodes.empty())
    {
        // get next pair
        auto data = animNodes.front();
        animNodes.pop();
        auto node = data.first;
        auto animNodeParent = data.second;
        // init current node
        auto animNode = std::make_shared<Animation::Node>();
        animNode->transform = Tools::convertAssimpMatrix(node->mTransformation);
        animNode->name = node->mName.C_Str();
        // add to parent node
        if (!animNodeParent)
            _animNodeRoot = animNode;
        else
            animNodeParent->children.push_back(animNode);
        // solve children nodes
        for (auto nodeIdx = 0u; nodeIdx < node->mNumChildren; ++nodeIdx)
            animNodes.push({node->mChildren[nodeIdx], animNode});
    }
    return true;
}

} // namespace RenderIt
