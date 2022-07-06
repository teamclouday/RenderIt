#include "Mesh.hpp"
#include "Material.hpp"

#include <cstddef>

namespace RenderIt
{

Mesh::Mesh() : _vao(nullptr), _vbo(nullptr), _ebo(nullptr), _mat(nullptr), _indicesCount(0)
{
}

Mesh::~Mesh()
{
    Reset();
}

void Mesh::Draw(const Shader *shader) const
{
    if (_mat)
    {
        shader->ConfigMaterialTextures(_mat.get());
        if (_mat->twoSided)
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
    }
    glActiveTexture(GL_TEXTURE0);
    _vao->Bind();
    glDrawElements(_primType, _indicesCount, GL_UNSIGNED_INT, 0);
    _vao->UnBind();
}

void Mesh::Load(MeshShape shape)
{
    if (_vao || _vbo || _ebo || _mat)
        Reset();
    // TODO: implement shapes
    switch (shape)
    {
    case MeshShape::Plane:
        break;
    case MeshShape::Cube:
        break;
    case MeshShape::Sphere:
        break;
    }
}

void Mesh::Load(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices,
                std::shared_ptr<Material> material, GLenum primType)
{
    if (_vao || _vbo || _ebo || _mat)
        Reset();
    _vao = std::make_unique<SVAO>();
    _vbo = std::make_unique<SBuffer>(GL_ARRAY_BUFFER);
    _ebo = std::make_unique<SBuffer>(GL_ELEMENT_ARRAY_BUFFER);
    _mat = material;
    _indicesCount = indices.size();
    _verticesCount = vertices.size();
    _primType = primType;

    _vao->Bind();

    _vbo->Bind();
    glBufferData(_vbo->type, _verticesCount * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    _ebo->Bind();
    glBufferData(_ebo->type, _indicesCount * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    // texcoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoords));
    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
    // bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
    // bone IDs
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));
    // bone weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, boneWeights));

    _vao->UnBind();
}

void Mesh::Reset()
{
    _vao = nullptr;
    _vbo = nullptr;
    _ebo = nullptr;
    _mat = nullptr;
}

std::optional<GLuint> Mesh::GetVertexArray()
{
    return _vao ? _vao->Get() : std::optional<GLuint>{std::nullopt};
}

std::optional<GLuint> Mesh::GetVertexBuffer()
{
    return _vbo ? _vbo->Get() : std::optional<GLuint>{std::nullopt};
}

std::optional<GLuint> Mesh::GetIndexBuffer()
{
    return _ebo ? _ebo->Get() : std::optional<GLuint>{std::nullopt};
}

size_t Mesh::GetNumVertices() const
{
    return _verticesCount;
}

size_t Mesh::GetNumIndices() const
{
    return _indicesCount;
}

} // namespace RenderIt
