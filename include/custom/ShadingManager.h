#ifndef SHADINGMANAGER_H
#define SHADINGMANAGER_H

#include <custom/Shader.h>

#include <imgui/imgui.h>

#include <map>
#include <string>

/*----------------------------------------------------------------------------------*/
/* SHADING MANAGER: Stores current shading style and implements shading style menu. */
/*----------------------------------------------------------------------------------*/
class ShadingManager {
private:
	Shader noShadingShader;

	Shader phongShader;

	Shader gourardShader;

	Shader toonShader;

    const char* shading[4] = {
        "Phong Shading",
        "Gourard",
        "Toon Shading",
        "No Shading"
    };

    std::map<std::string, Shader*> shadingMap;

    const char* currentShading;
    Shader* currentShader;

    ShadingManager() : 
        noShadingShader{ Shader("Shaders/NoShading.vert", "Shaders/NoShading.frag") }, 
        phongShader { Shader("Shaders/Phong.vert", "Shaders/Phong.frag") },
        gourardShader { Shader("Shaders/Gourard.vert", "Shaders/Gourard.frag") },
        toonShader { Shader("Shaders/Toon.vert", "Shaders/Toon.frag") }
    {
        shadingMap["Phong Shading"] = &phongShader;
        shadingMap["Gourard"] = &gourardShader;
        shadingMap["Toon Shading"] = &toonShader;
        shadingMap["No Shading"] = &noShadingShader;
        currentShading = shading[0];
        currentShader = shadingMap[currentShading];
    }

	void MenuImpl() {
        if (ImGui::CollapsingHeader("Shading")) {
            ImGui::Indent(20.0f);
            ImGui::Text("Shading Style");
            if (ImGui::BeginCombo("##Select Shading Style", currentShading)) {
                for (int n = 0; n < IM_ARRAYSIZE(shading); n++) {
                    bool is_selected = (shading[n] == currentShading);
                    if (ImGui::Selectable(shading[n], is_selected)) {
                        currentShading = shading[n];
                        currentShader = shadingMap[currentShading];
                    }
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Unindent();
        }
	}

    Shader* GetShaderImpl() {
        return currentShader;
    }

    void FreeImpl() {
        noShadingShader.deleteShader();
        phongShader.deleteShader();
        gourardShader.deleteShader();
        toonShader.deleteShader();
    }

public:
    ShadingManager(const ShadingManager&) = delete;
    static ShadingManager& getInstance() {
        static ShadingManager instance;
        return instance;
    }

    static void Menu() {
        getInstance().MenuImpl();
    }

    static Shader* GetShader() {
        return getInstance().GetShaderImpl();
    }

    static void Free() {
        getInstance().FreeImpl();
    }
};

#endif