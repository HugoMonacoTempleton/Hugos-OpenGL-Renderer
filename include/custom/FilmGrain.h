#ifndef FILMGRAIN_H
#define FILMGRAIN_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*--------------------------------------------------------*/
/* FILM GRAIN: Applies a film grain effect to the screen. */
/*--------------------------------------------------------*/
class FilmGrain : public PostProcessingEffect {
private:
	Shader filmGrainShader;

	Shader copyShader;

	Material filmGrainMat;

	// Effect properties
	float intensity;

public:
	FilmGrain() :
		filmGrainShader{ Shader("Shaders/PostProcess.vert", "Shaders/FilmGrain.frag") },
		copyShader{ Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") },
		filmGrainMat { Material(filmGrainShader) }
	{
		intensity = 0.0f;
	}

	~FilmGrain() {
		filmGrainShader.deleteShader();
		copyShader.deleteShader();
		filmGrainMat.deleteMaterial();
	}

	void Render(GBuffer& FrameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		filmGrainMat.setFloat("intensity", intensity);


		drawUtility::blit(FrameBuffer.screenBuffer, tmp, filmGrainMat);
		drawUtility::blit(tmp, FrameBuffer.screenBuffer, copyShader);

		tmp.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderFloat((std::string("Intensity##") + std::to_string(pos)).c_str(), &intensity, 0.0f, 10.0f);
	}
};

#endif