#include "Bounds.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "GLStructs.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Transform.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <memory>
#include <string>

namespace RenderIt
{

void UIShowMatrix(const glm::mat4 &m)
{
    ImGui::Text("%6.2f,%6.2f,%6.2f,%6.2f\n%6.2f,%6.2f,%6.2f,%6.2f\n%6.2f,%6.2f,%6.2f,%6.2f\n%6.2f,%6.2f,%6.2f,%6.2f",
                m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2],
                m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]);
}

void UIShowVector(const glm::vec3 &v)
{
    ImGui::Text("(%.2f,%.2f,%.2f)", v.x, v.y, v.z);
}

void UIShowTexture(const std::shared_ptr<STexture> &tex)
{
    if (!tex)
    {
        ImGui::Text("No Texture");
        return;
    }
    ImGui::Text("ID (%d)", tex->Get());
}

void UIShowMaterial(const std::shared_ptr<Material> &mat)
{
    if (!mat)
    {
        ImGui::Text("No Material");
        return;
    }
    if (ImGui::TreeNode("Textures"))
    {
        if (ImGui::TreeNode(Material::mapNameDiffuse.c_str()))
        {
            UIShowTexture(mat->diffuse);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameSpecular.c_str()))
        {
            UIShowTexture(mat->specular);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameAmbient.c_str()))
        {
            UIShowTexture(mat->ambient);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameEmissive.c_str()))
        {
            UIShowTexture(mat->emissive);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameHeight.c_str()))
        {
            UIShowTexture(mat->height);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameNormals.c_str()))
        {
            UIShowTexture(mat->normals);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameShininess.c_str()))
        {
            UIShowTexture(mat->shininess);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameOpacity.c_str()))
        {
            UIShowTexture(mat->opacity);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameDisplacement.c_str()))
        {
            UIShowTexture(mat->displacement);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameLightmap.c_str()))
        {
            UIShowTexture(mat->lightmap);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNameReflection.c_str()))
        {
            UIShowTexture(mat->reflection);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("PBR Textures"))
    {
        if (ImGui::TreeNode(Material::mapNamePBRColor.c_str()))
        {
            UIShowTexture(mat->pbr_color);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNamePBRNormal.c_str()))
        {
            UIShowTexture(mat->pbr_normal);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNamePBREmission.c_str()))
        {
            UIShowTexture(mat->pbr_emission);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNamePBRMetalness.c_str()))
        {
            UIShowTexture(mat->pbr_metalness);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNamePBRRoughness.c_str()))
        {
            UIShowTexture(mat->pbr_roughness);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::mapNamePBROcclusion.c_str()))
        {
            UIShowTexture(mat->pbr_occlusion);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Constants"))
    {
        if (ImGui::TreeNode(Material::valNameColorAmbient.c_str()))
        {
            UIShowVector(mat->colorAmbient);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::valNameColorDiffuse.c_str()))
        {
            UIShowVector(mat->colorDiffuse);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::valNameColorSpecular.c_str()))
        {
            UIShowVector(mat->colorSpecular);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::valNameColorEmissive.c_str()))
        {
            UIShowVector(mat->colorEmissive);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::valNameColorShininess.c_str()))
        {
            ImGui::Text("%.2f", mat->valShininess);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode(Material::valNameColorOpacity.c_str()))
        {
            ImGui::Text("%.2f", mat->valOpacity);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void UIShowBounds(const Bounds &b)
{
    if (ImGui::TreeNode("Max"))
    {
        UIShowVector(b.max);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Min"))
    {
        UIShowVector(b.min);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Center"))
    {
        UIShowVector(b.center);
        ImGui::TreePop();
    }
}

void UIEditTransform(Transform &t)
{
    bool shouldUpdate = false;
    shouldUpdate |= ImGui::DragFloat3("Position", glm::value_ptr(t.position), 0.001f, -10000.0f, 10000.0f, "%.3f");
    shouldUpdate |= ImGui::DragFloat3("Rotation", glm::value_ptr(t.rotation), 0.001f, -10000.0f, 10000.0f, "%.3f");
    shouldUpdate |= ImGui::DragFloat3("Scale", glm::value_ptr(t.scale), 0.001f, -10000.0f, 10000.0f, "%.3f");
    if (t.scale.x == t.scale.y && t.scale.x == t.scale.z)
    {
        if (ImGui::DragFloat("Scale All", &t.scale.x, 0.001f, -10000.0f, 10000.0f, "%.3f"))
        {
            t.scale.y = t.scale.x;
            t.scale.z = t.scale.x;
            shouldUpdate = true;
        }
    }
    if (shouldUpdate)
        t.UpdateMatrix();
    if (ImGui::TreeNode("Matrix"))
    {
        UIShowMatrix(t.matrix);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Matrix Inverse"))
    {
        UIShowMatrix(t.matrixInv);
        ImGui::TreePop();
    }
}

void AppContext::UI()
{
    ImGui::PushID(NAME.c_str());

    ImGui::Text("App [%s]", _winTitle.c_str());
    ImGui::Text("Window Size: (%dx%d)", _winW, _winH);
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
    ImGui::Text("Author: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.25f, 1.0f, 0.7f, 1.0f), "teamclouday");

    ImGui::PopID();
}

void Camera::UI()
{
    ImGui::PushID(NAME.c_str());

    ImGui::ColorEdit4("Clear Color", glm::value_ptr(clearColor));
    ImGui::Text("View Type");
    if (ImGui::RadioButton(std::to_string(CameraViewType::Ortho).c_str(), reinterpret_cast<int *>(&_viewType),
                           static_cast<int>(CameraViewType::Ortho)))
        _updated = false;
    if (ImGui::RadioButton(std::to_string(CameraViewType::Persp).c_str(), reinterpret_cast<int *>(&_viewType),
                           static_cast<int>(CameraViewType::Persp)))
        _updated = false;
    if (ImGui::DragFloat("Fov", &_fov, 0.01f, 0.0f, 180.0f, "%.2f"))
        _updated = false;
    if (ImGui::DragFloat("View Near", &_viewNear, 0.01f, 0.0f, _viewFar, "%.2f"))
        _updated = false;
    if (ImGui::DragFloat("View Far", &_viewFar, 0.01f, _viewNear, 10000.0f, "%.2f"))
        _updated = false;

    ImGui::Separator();

    if (ImGui::DragFloat3("Position", glm::value_ptr(_posVec), 0.001f, -10000.0f, 10000.0f, "%.3f"))
        _updated = false;
    if (ImGui::DragFloat3("Center", glm::value_ptr(_centerVec), 0.001f, -10000.0f, 10000.0f, "%.3f"))
        _updated = false;
    ImGui::Text("Distance: %.5f", _dist);

    ImGui::Separator();

    if (ImGui::TreeNode("Up"))
    {
        UIShowVector(_upVec);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Front"))
    {
        UIShowVector(_frontVec);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Right"))
    {
        UIShowVector(_rightVec);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("World Up"))
    {
        UIShowVector(_worldUpVec);
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Projection Matrix"))
    {
        UIShowMatrix(_projMat);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("View Matrix"))
    {
        UIShowMatrix(_viewMat);
        ImGui::TreePop();
    }

    ImGui::Separator();

    ImGui::Text("Aspect Ratio: %.4f", _aspect);
    ImGui::Text("Updated: %s", _updated ? "true" : "false");

    ImGui::PopID();
}

void Mesh::UI()
{
    ImGui::PushID(NAME.c_str());

    ImGui::Text("VAO ID (%d)", _vao ? _vao->Get() : 0);
    ImGui::Text("VBO ID (%d)", _vbo ? _vbo->Get() : 0);
    ImGui::Text("EBO ID (%d)", _ebo ? _ebo->Get() : 0);
    ImGui::Text("Face Count = %d", _indicesCount);

    if (ImGui::TreeNode("Material"))
    {
        UIShowMaterial(_mat);
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Model::UI()
{
    ImGui::PushID(NAME.c_str());

    ImGui::Text("Name: %s", modelName.c_str());

    ImGui::Separator();

    if (ImGui::TreeNode("Transform"))
    {
        UIEditTransform(transform);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Bounds"))
    {
        UIShowBounds(bounds);
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::TreeNode("Meshes"))
    {
        for (auto i = 0; i < _meshes.size(); i++)
        {
            ImGui::PushID(i);
            _meshes[i]->UI();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (_children.size() && ImGui::TreeNode("Children"))
    {
        for (auto i = 0; i < _children.size(); i++)
        {
            ImGui::PushID(i);
            _children[i]->UI();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    // TODO: Add animation GUI

    ImGui::PopID();
}

} // namespace RenderIt
