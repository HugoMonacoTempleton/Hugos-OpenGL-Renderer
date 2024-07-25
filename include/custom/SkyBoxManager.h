#ifndef SKYBOXMANAGER_H
#define SKYBOXMANAGER_H

#include <custom/Shader.h>
#include <custom/TexUtility.h>
#include <custom/BaseMesh.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <map>
#include <string>

/*-------------------------------------------------------------------------------------------------------*/
/* SKYBOX MANAGER: Stores and renders the scene skybox, as well as implementing the skybox settings menu.*/
/*-------------------------------------------------------------------------------------------------------*/
class SkyBoxManager {
private:
    Shader skyBoxShader; 
    
    unsigned int skyBoxVAO;

    const char* skyBoxes[3] = {
        "Mountains",
        "Ocean",
        "Land"
    };

    unsigned int mountainsTexture = texUtility::cubemapFromFile("Textures/Mountains");
    unsigned int clearOceanTexture = texUtility::cubemapFromFile("Textures/ClearOcean");
    unsigned int classicLandTexture = texUtility::cubemapFromFile("Textures/ClassicLand");

    std::map<std::string, unsigned int> skyBoxMap;

    bool enableSkyBox = true;
    const char* skyBoxLabel;
    unsigned int currSkyBox;

    glm::vec3 backgroundColor = glm::vec3(0.1, 0.1, 0.1);

    SkyBoxManager() :
        skyBoxShader { Shader("Shaders/skyBoxShader.vert", "Shaders/skyBoxShader.frag") }
    {
        skyBoxMap["Mountains"] = mountainsTexture;
        skyBoxMap["Ocean"] = clearOceanTexture;
        skyBoxMap["Land"] = classicLandTexture;
        skyBoxLabel = skyBoxes[0];
        currSkyBox = skyBoxMap[skyBoxLabel];
        BaseMesh& baseMesh = BaseMesh::getInstance();
        skyBoxVAO = baseMesh.skyBoxVAO;
    }
    void MenuImpl() {
        if (ImGui::CollapsingHeader("Sky Box")) {
            ImGui::Indent(20.0f);
            ImGui::Checkbox("Enable Sky Box", &enableSkyBox);
            if (enableSkyBox) {
                ImGui::Indent(20.0f);
                if (ImGui::BeginCombo("Select Sky Box", skyBoxLabel)) {
                    for (int n = 0; n < IM_ARRAYSIZE(skyBoxes); n++) {
                        bool is_selected = (skyBoxes[n] == skyBoxLabel);
                        if (ImGui::Selectable(skyBoxes[n], is_selected)) {
                            skyBoxLabel = skyBoxes[n];
                            currSkyBox = skyBoxMap[skyBoxLabel];
                        }
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::Unindent();
            }
            else {
                ImGui::ColorPicker3("Background Color", glm::value_ptr(backgroundColor));
            }
            ImGui::Unindent();
        }
    }
    void DrawImpl() {
        if (enableSkyBox) {
            glDepthFunc(GL_LEQUAL);
            skyBoxShader.use();
            glBindVertexArray(skyBoxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, currSkyBox);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
        }
    }

    void SetClearColorImpl() {
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0);
    }

    void FreeImpl() {
        skyBoxShader.deleteShader();
        glDeleteTextures(1, &mountainsTexture);
        glDeleteTextures(1, &clearOceanTexture);
        glDeleteTextures(1, &classicLandTexture);
    }

public:
    SkyBoxManager(const SkyBoxManager&) = delete;
    static SkyBoxManager& getInstance() {
        static SkyBoxManager instance;
        return instance;
    }

    static void Draw() {
        getInstance().DrawImpl();
    }

    static void Menu() {
        getInstance().MenuImpl();
    }

    static void SetClearColor() {
        getInstance().SetClearColorImpl();
    }

    static void Free() {
        getInstance().FreeImpl();
    }
};

#endif