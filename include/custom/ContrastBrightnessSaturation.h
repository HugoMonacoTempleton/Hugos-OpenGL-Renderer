#ifndef CONTRASTBRIGHTNESSSATURATION_H
#define CONTRASTBRIGHTNESSSATURATION_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*----------------------------------------------------------------------------------------------------------*/
/* CONTRAST BRIGHTNESS SATURATION: Applies contrast, brightness and saturation color correction the screen. */
/*----------------------------------------------------------------------------------------------------------*/
class ContrastBrightnessSaturation : public PostProcessingEffect {
private:
	Shader ContrastBrightnessSaturationShader;

	Shader copyShader;

	Material ContrastBrightnessSaturationMat;

	// Effect properties
	float contrast;

	float brightness;

	float saturation;

public:
	ContrastBrightnessSaturation() :
		ContrastBrightnessSaturationShader{ Shader("Shaders/PostProcess.vert", "Shaders/ContrastBrightnessSaturation.frag") },
		copyShader{ Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") },
		ContrastBrightnessSaturationMat{ Material(ContrastBrightnessSaturationShader) }
	{
		contrast = 1.0;
		brightness = 0.0;
		saturation = 1.0;
	}

	~ContrastBrightnessSaturation() {
		ContrastBrightnessSaturationShader.deleteShader();
		ContrastBrightnessSaturationMat.deleteMaterial();
		copyShader.deleteShader();
	}

	void Render(GBuffer& FrameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		ContrastBrightnessSaturationMat.setFloat("contrast", contrast);
		ContrastBrightnessSaturationMat.setFloat("brightness", brightness);
		ContrastBrightnessSaturationMat.setFloat("saturation", saturation);

		drawUtility::blit(FrameBuffer.screenBuffer, tmp, ContrastBrightnessSaturationMat);
		drawUtility::blit(tmp, FrameBuffer.screenBuffer, copyShader);

		tmp.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderFloat((std::string("Contrast##") + std::to_string(pos)).c_str(), &contrast, 0.0f, 10.0f);
		ImGui::SliderFloat((std::string("Brightness##") + std::to_string(pos)).c_str(), &brightness, 0.0f, 10.0f);
		ImGui::SliderFloat((std::string("Saturation##") + std::to_string(pos)).c_str(), &saturation, 0.0f, 2.0f);
	}
};

#endif