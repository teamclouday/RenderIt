#include "Animation.hpp"
#include "Bone.hpp"
#include "Bounds.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "GLStructs.hpp"
#include "Lights.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Scene.hpp"
#include "Transform.hpp"

#include "Cameras/FreeCamera.hpp"
#include "Cameras/OrbitCamera.hpp"

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

void UIEditMaterial(std::shared_ptr<Material> &mat)
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
    if (ImGui::TreeNode("Values"))
    {
        ImGui::ColorEdit3(Material::valNameColorAmbient.c_str(), glm::value_ptr(mat->colorAmbient));
        ImGui::ColorEdit3(Material::valNameColorDiffuse.c_str(), glm::value_ptr(mat->colorDiffuse));
        ImGui::ColorEdit3(Material::valNameColorSpecular.c_str(), glm::value_ptr(mat->colorSpecular));
        ImGui::ColorEdit3(Material::valNameColorEmissive.c_str(), glm::value_ptr(mat->colorEmissive));
        ImGui::DragFloat(Material::valNameColorShininess.c_str(), &mat->valShininess, 0.01f, 0.0f, 1000.0f, "%.2f");
        ImGui::DragFloat(Material::valNameColorOpacity.c_str(), &mat->valOpacity, 0.001f, 0.0f, 1.0f, "%.3f");
        ImGui::Checkbox("Two Sided", &mat->twoSided);
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

void UIShowBone(const Bone &bone)
{
    ImGui::Text("Name = %s", bone.name.c_str());
    ImGui::Text("ID = %u", bone.ID);
    ImGui::Separator();
    if (ImGui::TreeNode("Transform"))
    {
        UIShowMatrix(bone.matrix);
        ImGui::TreePop();
    }
}

void UIShowAnimation(const Animation &anim)
{
    ImGui::Text("Name: %s", anim.name.c_str());
    ImGui::Separator();
    ImGui::Text("Current: %.2f (s)", anim.currTime);
    ImGui::Text("Duration: %.2f (s)", anim.duration);
    ImGui::Text("Ticks: %1f (per second)", anim.ticksPerSecond);
    ImGui::Separator();
    ImGui::Text("Max Supported Bones: %d", ANIMATION_MAX_BONES);
    if (ImGui::TreeNode("Bones"))
    {
        for (auto &pair : anim.bones)
        {
            ImGui::PushID(pair.first.c_str());
            if (ImGui::TreeNode(pair.first.c_str()))
            {
                UIShowBone(*pair.second.get());
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

void UIShowAnimNode(const Animation::Node &node)
{
    if (ImGui::TreeNode(node.name.c_str()))
    {
        if (ImGui::TreeNode("Transform"))
        {
            UIShowMatrix(node.transform);
            ImGui::TreePop();
        }
        if (node.children.size() && ImGui::TreeNode("Children"))
        {
            for (auto i = 0; i < node.children.size(); ++i)
            {
                ImGui::PushID(i);
                UIShowAnimNode(*node.children[i].get());
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void AppContext::UI()
{
    ImGui::PushID(LOGNAME.c_str());

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
    ImGui::PushID(LOGNAME.c_str());

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

void OrbitCamera::UI()
{
    Camera::UI();
    ImGui::PushID(LOGNAME.c_str());

    ImGui::Separator();
    ImGui::Text("Input Controls");

    if (ImGui::TreeNode("Rotation (Mouse)"))
    {
        ImGui::DragFloat("Sensitivity", &sensRotateM, 0.001f, 0.001f, 10000.0f, "%.3f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Move (Mouse)"))
    {
        ImGui::DragFloat("Sensitivity", &sensMoveM, 0.001f, 0.001f, 10000.0f, "%.3f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Move (Key)"))
    {
        ImGui::DragFloat("Sensitivity", &sensMoveK, 0.001f, 0.001f, 10000.0f, "%.3f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Zoom (Wheel)"))
    {
        ImGui::DragFloat("Min Distance", &wheelZoomMinDist, 0.0001f, 0.0001f, 10000.0f, "%.4f");
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void FreeCamera::UI()
{
    Camera::UI();
    ImGui::PushID(LOGNAME.c_str());

    ImGui::Separator();
    ImGui::Text("Input Controls");

    if (ImGui::TreeNode("Rotation (Mouse)"))
    {
        ImGui::DragFloat("Sensitivity", &sensRotateM, 0.001f, 0.001f, 10000.0f, "%.3f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Move (Key)"))
    {
        ImGui::DragFloat("Sensitivity", &sensMoveK, 0.001f, 0.001f, 10000.0f, "%.3f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Zoom (Wheel)"))
    {
        ImGui::DragFloat("Min Distance", &wheelZoomMinDist, 0.0001f, 0.0001f, 10000.0f, "%.4f");
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Mesh::UI()
{
    ImGui::PushID(LOGNAME.c_str());

    ImGui::Text("VAO ID (%d)", _vao ? _vao->Get() : 0);
    ImGui::Text("VBO ID (%d)", _vbo ? _vbo->Get() : 0);
    ImGui::Text("EBO ID (%d)", _ebo ? _ebo->Get() : 0);
    ImGui::Text("Face Count = %d", _indicesCount);

    if (ImGui::TreeNode("Material"))
    {
        UIEditMaterial(_mat);
        ImGui::TreePop();
    }

    ImGui::PopID();
}

void Model::UI()
{
    ImGui::PushID(LOGNAME.c_str());

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
        for (auto i = 0; i < _meshes.size(); ++i)
        {
            ImGui::PushID(i);
            if (ImGui::TreeNode(("Mesh " + std::to_string(i)).c_str()))
            {
                _meshes[i]->UI();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (_animations.size() && ImGui::TreeNode("Animations"))
    {
        for (auto i = 0; i < _animations.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::RadioButton(_animations[i]->name.c_str(), reinterpret_cast<int *>(&_animationActive),
                               static_cast<int>(i));
            if (ImGui::TreeNode((_animations[i]->name + " details").c_str()))
            {
                UIShowAnimation(*_animations[i].get());
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (_animNodeRoot && ImGui::TreeNode("Animation Tree"))
    {
        UIShowAnimNode(*_animNodeRoot.get());
        ImGui::TreePop();
    }

    ImGui::Separator();

    if (_children.size() && ImGui::TreeNode("Children"))
    {
        for (auto i = 0; i < _children.size(); ++i)
        {
            ImGui::PushID(i);
            _children[i]->UI();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

bool UIEditDirLight(DirLight &light)
{
    bool updated = false;
    updated |= ImGui::DragFloat3("Direction", glm::value_ptr(light.dir), 0.01f);
    updated |= ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    updated |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f);
    bool castShadow = static_cast<bool>(light.castShadow);
    if (ImGui::Checkbox("Cast Shadow", &castShadow))
    {
        light.castShadow = static_cast<int>(castShadow);
        updated = true;
    }
    return updated;
}

bool UIEditPointLight(PointLight &light)
{
    bool updated = false;
    updated |= ImGui::DragFloat3("Position", glm::value_ptr(light.pos), 0.01f);
    updated |= ImGui::DragFloat("Range", &light.range, 0.01f, 0.0f);
    updated |= ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    updated |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f);
    bool castShadow = static_cast<bool>(light.castShadow);
    if (ImGui::Checkbox("Cast Shadow", &castShadow))
    {
        light.castShadow = static_cast<int>(castShadow);
        updated = true;
    }
    return updated;
}

bool UIEditSpotLight(SpotLight &light)
{
    bool updated = false;
    updated |= ImGui::DragFloat3("Position", glm::value_ptr(light.pos), 0.01f);
    updated |= ImGui::DragFloat3("Direction", glm::value_ptr(light.dir), 0.01f);
    updated |= ImGui::DragFloat("Range", &light.range, 0.01f, 0.0f);
    updated |= ImGui::DragFloat("Cutoff (Inner)", &light.cutoffInner, 0.001f, 0.0f, light.cutoffOuter, "%.3f");
    updated |= ImGui::DragFloat("Cutoff (Outer)", &light.cutoffOuter, 0.001f, light.cutoffInner, 1.0f, "%.3f");
    updated |= ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
    updated |= ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f);
    bool castShadow = static_cast<bool>(light.castShadow);
    if (ImGui::Checkbox("Cast Shadow", &castShadow))
    {
        light.castShadow = static_cast<int>(castShadow);
        updated = true;
    }
    return updated;
}

void LightManager::UI()
{
    ImGui::PushID(LOGNAME.c_str());

    ImGui::PushID("DirLight");
    if (ImGui::TreeNode("Directional"))
    {
        for (auto i = 0u; i < _dirLights.size(); ++i)
        {
            ImGui::PushID(i);
            if (ImGui::TreeNode(("Light " + std::to_string(i)).c_str()))
            {
                _dirLightsSSBOUpdated &= !UIEditDirLight(_dirLights[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::Button("Push"))
        PushLight(LightType::Directional);
    ImGui::SameLine();
    if (ImGui::Button("Pop"))
        PopLight(LightType::Directional);
    ImGui::PopID();

    ImGui::Separator();

    ImGui::PushID("PointLight");
    if (ImGui::TreeNode("Point"))
    {
        for (auto i = 0u; i < _pointLights.size(); ++i)
        {
            ImGui::PushID(i);
            if (ImGui::TreeNode(("Light " + std::to_string(i)).c_str()))
            {
                _pointLightsSSBOUpdated &= !UIEditPointLight(_pointLights[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::Button("Push"))
        PushLight(LightType::Point);
    ImGui::SameLine();
    if (ImGui::Button("Pop"))
        PopLight(LightType::Point);
    ImGui::PopID();

    ImGui::Separator();

    ImGui::PushID("SpotLightLight");
    if (ImGui::TreeNode("Spot"))
    {
        for (auto i = 0u; i < _spotLights.size(); ++i)
        {
            ImGui::PushID(i);
            if (ImGui::TreeNode(("Light " + std::to_string(i)).c_str()))
            {
                _spotLightsSSBOUpdated &= !UIEditSpotLight(_spotLights[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::Button("Push"))
        PushLight(LightType::Spot);
    ImGui::SameLine();
    if (ImGui::Button("Pop"))
        PopLight(LightType::Spot);
    ImGui::PopID();

    ImGui::PopID();
}

void Scene::UI()
{
    ImGui::PushID(LOGNAME.c_str());
    auto iter = models.begin();
    auto index = 0u;
    while (iter != models.end())
    {
        ImGui::PushID(index);
        if (ImGui::TreeNode(("Model " + std::to_string(index)).c_str()))
        {
            (*iter)->UI();
            ImGui::TreePop();
        }
        ImGui::PopID();
        iter++;
    }
    ImGui::PopID();
}

} // namespace RenderIt
