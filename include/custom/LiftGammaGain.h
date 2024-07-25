#ifndef LIFTGAMMAGAIN_H
#define LIFTGAMMAGAIN_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*------------------------------------------------------------------------------------*/
/* LIFT GAMMA GAIN: Applies lift, gain and gamma color grading filters to the screen. */
/*------------------------------------------------------------------------------------*/
class LiftGammaGain : public PostProcessingEffect {
private :
	Shader liftGammaGainShader;

	Shader copyShader;

	Material liftGammaGainMat;

	// Effect properties
	float lift;

	float gamma;

	float gain;
public:
	LiftGammaGain() :
		liftGammaGainShader { Shader("Shaders/PostProcess.vert", "Shaders/LiftGammaGain.frag") },
		copyShader { Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") },
		liftGammaGainMat{ Material(liftGammaGainShader) }
	{
		lift = 0.0;
		gamma = 1.0;
		gain = 1.0;
	}

	~LiftGammaGain()
	{
		liftGammaGainShader.deleteShader();
		copyShader.deleteShader();
		liftGammaGainMat.deleteMaterial();
	}

	void Render(GBuffer& frameBuffer, int SCR_WIDTH, int SCR_HEIGHT) {
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		liftGammaGainMat.setFloat("lift", lift);
		liftGammaGainMat.setFloat("gamma", gamma);
		liftGammaGainMat.setFloat("gain", gain);

		drawUtility::blit(frameBuffer.screenBuffer, tmp, liftGammaGainMat);

		drawUtility::blit(tmp, frameBuffer.screenBuffer, copyShader);

		tmp.deleteRenderTex();
	}

	void Menu(int pos) {
		ImGui::SliderFloat((std::string("Lift##") + std::to_string(pos)).c_str(), &lift, 0.0f, 1.0f);
		ImGui::SliderFloat((std::string("Gamma##") + std::to_string(pos)).c_str(), &gamma, 0.01f, 8.0f);
		ImGui::SliderFloat((std::string("Gain##") + std::to_string(pos)).c_str(), &gain, 0.0f, 8.0f);
	}
};
#endif