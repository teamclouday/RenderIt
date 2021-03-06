#include "Mesh.hpp"
#include "Material.hpp"

#include <cstddef>

namespace RenderIt
{

Mesh::Mesh()
    : material(nullptr), primType(GL_TRIANGLES), drawMesh(true), _vao(nullptr), _vbo(nullptr), _ebo(nullptr),
      _indicesCount(0), _verticesCount(0)
{
}

Mesh::~Mesh()
{
    Reset();
}

void Mesh::Draw(const Shader *shader, const RenderPass &pass) const
{
    if (!_vao || !_indicesCount || !drawMesh)
        return;
    auto hasBlend = glIsEnabled(GL_BLEND);
    auto hasCullFace = glIsEnabled(GL_CULL_FACE);
    auto isTransparent = false;
    if (material)
    {
        // if unordered, skip all checking
        if (pass != RenderPass::AllUnOrdered)
        {
            // check render pass for transparency
            isTransparent = material->valOpacity < 1.0f || material->opacity || material->alphaMode == 1;
            if ((pass == RenderPass::Transparent) != isTransparent)
                return;
            // check render pass for refraction
            auto isRefract = material->valRefract != 1.0f;
            if ((pass == RenderPass::Transmissive) != isRefract)
                return;
        }
        // configure material
        shader->ConfigMaterialTextures(material.get());
        if (material->twoSided)
            glDisable(GL_CULL_FACE);
        else
            glEnable(GL_CULL_FACE);
    }
    _vao->Bind();
    if (isTransparent)
    {
        // for transparent meshes, render back face and then front face
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glDrawElements(primType, static_cast<GLsizei>(_indicesCount), GL_UNSIGNED_INT, 0);
        glCullFace(GL_BACK);
        glDrawElements(primType, static_cast<GLsizei>(_indicesCount), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDisable(GL_BLEND);
        glDrawElements(primType, static_cast<GLsizei>(_indicesCount), GL_UNSIGNED_INT, 0);
    }
    _vao->UnBind();
    if (hasCullFace)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
    if (hasBlend)
        glEnable(GL_BLEND);
}

void Mesh::Load(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices,
                std::shared_ptr<Material> mat, GLenum type)
{
    if (_vao || _vbo || _ebo)
        Reset();
    _vao = std::make_unique<SVAO>();
    _vbo = std::make_unique<SBuffer>(GL_ARRAY_BUFFER);
    _ebo = std::make_unique<SBuffer>(GL_ELEMENT_ARRAY_BUFFER);
    material = mat;
    _indicesCount = indices.size();
    _verticesCount = vertices.size();
    primType = type;

    _vao->Bind();
    _vbo->Bind();
    glBufferData(_vbo->type, _verticesCount * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    _ebo->Bind();
    glBufferData(_ebo->type, _indicesCount * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
    setupVAOAttributes();
    _vao->UnBind();
}

void Mesh::Reset()
{
    _vao = nullptr;
    _vbo = nullptr;
    _ebo = nullptr;
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

void Mesh::setupVAOAttributes() const
{
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
    // vertex color
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
}

} // namespace RenderIt
