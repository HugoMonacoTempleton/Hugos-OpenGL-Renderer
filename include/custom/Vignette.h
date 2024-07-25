#ifndef VIGNETTE_H
#define VIGNETTE_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*-----------------------------------------------------*/
/* VIGNETTE: Draws a vignette overlay over the screen. */
/*-----------------------------------------------------*/
class Vignette : public PostProcessingEffect {
private:
	Shader vignetteShader;

	Shader copyShader;

	Material vignetteMat;

	// Effect properties
	float radius;

	float focus;
public:
	Vignette() :
		vignetteShader { Shader("Shaders/PostProcess.vert", "Shaders/Vignette.frag")},
		copyShader { Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag")},
		vignetteMat { Material(vignetteShader)}
	{
		radius = 1.0f;
		focus = 1.0f;
	}

	~Vignette()
	{
		vignetteShader.deleteShader();
		copyShader.deleteShader();
		vignetteMat.deleteMaterial();
	}

	void Render(GBuffer& frameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		vignetteMat.setFloat("radius", radius);
		vignetteMat.setFloat("focus", focus);

		drawUtility::blit(frameBuffer.screenBuffer, tmp, vignetteMat);
		drawUtility::blit(tmp, frameBuffer.screenBuffer, copyShader);

		tmp.deleteRenderTex();
	}

	void Menu(int pos) {
		ImGui::SliderFloat((std::string("Radius##") + std::to_string(pos + 1)).c_str(), &radius, 0.0f, 2.5f);
		ImGui::SliderFloat((std::string("Focus##") + std::to_string(pos + 1)).c_str(), &focus, 1.0f, 32.0f);
	}
};

#endif
