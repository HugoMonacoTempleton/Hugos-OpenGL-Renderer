#ifndef POSTPROCESSINGMANAGER_H
#define POSTPROCESSINGMANAGER_H

#include <custom/NormalDepthEdgeDetection.h>
#include <custom/ContrastBrightnessSaturation.h>
#include <custom/FilmGrain.h>
#include <custom/Vignette.h>
#include <custom/ChromaticAberration.h>
#include <custom/PixelDither.h>
#include <custom/LiftGammaGain.h>
#include <custom/SobelEdgeDetection.h>
#include <custom/Camera.h>
#include <custom/GBuffer.h>
#include <custom/PostProcessingEffect.h>

#include <imgui/imgui.h>

#include <string>
#include <vector>
#include <map>

/*-------------------------------------------------------------------------------------------------------------------------------------*/
/* POST PROCESSING MANAGER: Stores and renders the post processing pipeline, as well as implementing the post processing settings menu.*/
/*-------------------------------------------------------------------------------------------------------------------------------------*/
class PostProcessingManager {
private:
    const char* effects[9] = {
        "Contrast Brightness Saturation",
        "Lift Gamma Gain",
        "Film Grain",
        "Vignette",
        "Chromatic Aberration",
        "Sobel Edge Detection",
        "Normal Depth Edge Detection",
        "Pixel Dither",
        "Undefined"
    };

    std::map<std::string, int> effectMap;
    
    std::vector<PostProcessingEffect*> effectPipeline;

    std::vector<std::string> effectNames = {};

    bool enabled = true;

	PostProcessingManager() {
        effectMap["Contrast Brightness Saturation"] = 0;
        effectMap["Lift Gamma Gain"] = 1;
        effectMap["Film Grain"] = 2;
        effectMap["Vignette"] = 3;
        effectMap["Chromatic Aberration"] = 4;
        effectMap["Sobel Edge Detection"] = 5;
        effectMap["Normal Depth Edge Detection"] = 6;
        effectMap["Pixel Dither"] = 7;
	}

	void RenderImpl(GBuffer& frameBuffer, Camera& camera) {
        if (enabled) {
            int width = camera.GetWidth();
            int height = camera.GetHeight();
            for (int i = 0; i < effectPipeline.size(); i++) {
                effectPipeline[i]->Render(frameBuffer, width, height);
            }
        }
        
	}

    void MenuImpl() {
        if (ImGui::CollapsingHeader("Post Processing")) {
            ImGui::Indent(20.0f);
            if (ImGui::Button("Add Effect")) {
                effectNames.push_back(effects[0]);
                effectPipeline.push_back(CreateEffect(0));
            }
            ImGui::Checkbox("Enable## Effects", &enabled);
            int moveTo = -1;
            int moveFrom = -1;
            for (int i = 0; i < effectNames.size(); i++) {
                if (ImGui::Selectable(effectNames[i].c_str())) {

                }

                ImGuiDragDropFlags src_flags = 0;
                src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
                src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
                src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip;

                if (ImGui::BeginDragDropSource(src_flags)) {
                    if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                        ImGui::Text("Moving \"%s", effectNames[i]);
                    ImGui::SetDragDropPayload("Post-Processing Effects", &i, sizeof(int));
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget()) {
                    ImGuiDragDropFlags target_flags = 0;
                    target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
                    target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Post-Processing Effects", target_flags)) {
                        moveFrom = *(const int*)payload->Data;
                        moveTo = i;
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::BeginCombo((std::string("Select Effect##") + std::to_string(i + 1)).c_str(), effectNames[i].c_str())) {
                    for (int n = 0; n < IM_ARRAYSIZE(effects); n++) {
                        bool is_selected = (effects[n] == effectNames[i].c_str());
                        if (ImGui::Selectable(effects[n], is_selected)) {
                            if (effectNames[i] != effects[n]) {
                                effectNames[i] = effects[n];
                                delete effectPipeline[i];
                                effectPipeline[i] = CreateEffect(effectMap[effects[n]]);
                            }

                        }
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::CollapsingHeader((std::string("Effect Properties##") + std::to_string(i + 1)).c_str())) {
                    effectPipeline[i]->Menu(i);
                }
                

                if (ImGui::Button((std::string("Remove Effect##") + std::to_string(i + 1)).c_str())) {
                    effectNames.erase(effectNames.begin() + i);
                    PostProcessingEffect* tmp = effectPipeline[i];
                    effectPipeline.erase(effectPipeline.begin() + i);
                    delete tmp;
                    i--;
                }

            }
            if (moveTo != -1 && moveFrom != -1) {
                int copyDst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
                int copySrc = (moveFrom < moveTo) ? moveTo + 1 : moveFrom;
                int copyCount = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
                std::string tmpName = effectNames[moveFrom];
                PostProcessingEffect* tmpEffect = effectPipeline[moveFrom];
                effectNames[moveFrom] = effectNames[moveTo];
                effectPipeline[moveFrom] = effectPipeline[moveTo];
                effectNames[moveTo] = tmpName;
                effectPipeline[moveTo] = tmpEffect;
                ImGui::SetDragDropPayload("Post-Processing Effects", &moveTo, sizeof(int));
            }
            ImGui::Unindent();

        }
    }

    void FreeImpl() {
        for (int i = 0; i < effectPipeline.size(); i++) {
            delete effectPipeline[i];
        }
    }

    PostProcessingEffect* CreateEffect(int label) {
        switch (label) {
        case 0:
            return new ContrastBrightnessSaturation();
            break;
        case 1:
            return new LiftGammaGain();
            break;
        case 2:
            return new FilmGrain();
            break;
        case 3:
            return new Vignette();
            break;
        case 4:
            return new ChromaticAberration();
            break;
        case 5:
            return new SobelEdgeDetection();
            break;
        case 6:
            return new NormalDepthEdgeDetection();
            break;
        case 7:
            return new PixelDither();
            break;
        }
        return NULL;
    }

public:
    PostProcessingManager(const PostProcessingManager&) = delete;
    static PostProcessingManager& getInstance() {
        static PostProcessingManager instance;
        return instance;
    }

    static void Render(GBuffer& frameBuffer, Camera& camera) {
        getInstance().RenderImpl(frameBuffer, camera);
    }

    static void Menu() {
        getInstance().MenuImpl();
    }

    static void Free() {
        getInstance().FreeImpl();
    }
};

#endif