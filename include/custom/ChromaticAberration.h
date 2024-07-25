#ifndef CHROMATICABERRATION_H
#define CHROMATICABERRATION_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <glm/glm.hpp>

#include <imgui/imgui.h>

#include <string>

/*----------------------------------------------------------------------------*/
/* CHROMATIC ABERRATION: Applies a chromatic aberration effect to the screen. */
/*----------------------------------------------------------------------------*/
class ChromaticAberration : public PostProcessingEffect {
private:
	Shader chromaticAberrationShader;

	Shader copyShader;

	Material chromaticAberrationMat;

	// Effect properties
	float radius;

	float focus;

	float redOffsetAngle;
	float redOffsetStrength;

	float greenOffsetAngle;
	float greenOffsetStrength;

	float blueOffsetAngle;
	float blueOffsetStrength;

public:
	ChromaticAberration() :
		chromaticAberrationShader{ Shader("Shaders/PostProcess.vert", "Shaders/ChromaticAberration.frag") },
		copyShader{ Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") },
		chromaticAberrationMat{ Material(chromaticAberrationShader) }
	{
		radius = 1.0f;

		focus = 1.0f;

		redOffsetAngle = 0.0f;
		redOffsetStrength = 1.0f;

		greenOffsetAngle = 120.0f;
		greenOffsetStrength = 1.0f;

		blueOffsetAngle = 240.0f;
		blueOffsetStrength = 1.0f;
	}

	void Render(GBuffer& frameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		chromaticAberrationMat.setFloat("radius", radius);
		chromaticAberrationMat.setFloat("focus", focus);
		chromaticAberrationMat.setFloat("redOffsetAngle", glm::radians(redOffsetAngle));
		chromaticAberrationMat.setFloat("redOffsetStrength", redOffsetStrength);
		chromaticAberrationMat.setFloat("greenOffsetAngle", glm::radians(greenOffsetAngle));
		chromaticAberrationMat.setFloat("greenOffsetStrength", greenOffsetStrength);
		chromaticAberrationMat.setFloat("blueOffsetAngle", glm::radians(blueOffsetAngle));
		chromaticAberrationMat.setFloat("blueOffsetStrength", blueOffsetStrength);

		drawUtility::blit(frameBuffer.screenBuffer, tmp, chromaticAberrationMat);
		drawUtility::blit(tmp, frameBuffer.screenBuffer, copyShader);

		tmp.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderFloat((std::string("Radius##") + std::to_string(pos + 1)).c_str(), &radius, 0.0f, 2.5f);
		ImGui::SliderFloat((std::string("Focus##") + std::to_string(pos + 1)).c_str(), &focus, 0.01f, 32.0f);
		ImGui::SliderFloat((std::string("Red Offset Angle##") + std::to_string(pos + 1)).c_str(), &redOffsetAngle, 0.0f, 360.0f);
		ImGui::SliderFloat((std::string("Red Offset Strength##") + std::to_string(pos + 1)).c_str(), &redOffsetStrength, 0.0f, 10.0f);
		ImGui::SliderFloat((std::string("Green Offset Angle##") + std::to_string(pos + 1)).c_str(), &greenOffsetAngle, 0.0f, 360.0f);
		ImGui::SliderFloat((std::string("Green Offset Strength##") + std::to_string(pos + 1)).c_str(), &greenOffsetStrength, 0.0f, 10.0f);
		ImGui::SliderFloat((std::string("Blue Offset Angle##") + std::to_string(pos + 1)).c_str(), &blueOffsetAngle, 0.0f, 360.0f);
		ImGui::SliderFloat((std::string("Blue Offset Strength##") + std::to_string(pos + 1)).c_str(), &blueOffsetStrength, 0.0f, 10.0f);
		
	}
};

#endif