#ifndef PIXELDITHER_H
#define PIXELDITHER_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <glm/glm.hpp>

#include <string>

/*-----------------------------------------------------------------------------------------*/
/* PIXEL DITHER: Pixelates and posterizes the screen output as well as applying dithering. */
/*-----------------------------------------------------------------------------------------*/
class PixelDither : public PostProcessingEffect {
private:
	Shader ditherShader;

	Shader copyShader;

	Material ditherMat;

	// Effect properties
	int height;

	float spread;

	int redColorCount;

	int greenColorCount;

	int blueColorCount;

	int bayerLevel;
public:
	PixelDither() :
		ditherShader { Shader("Shaders/PostProcess.vert", "Shaders/Dither.frag") },
		copyShader { Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") },
		ditherMat { Material(ditherShader) }
	{
		height = 128;

		spread = 0.05;

		redColorCount = 16;

		greenColorCount = 16;

		blueColorCount = 16;

		bayerLevel = 0;
	}

	~PixelDither()
	{
		ditherShader.deleteShader();
		copyShader.deleteShader();
		ditherMat.deleteMaterial();
	}

	void Render(GBuffer& frameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		int width = (int) glm::floor(height * ((float) SCR_WIDTH )/ (float) SCR_HEIGHT);

		RenderTexture pixelPass = RenderTexture(width, height);
		RenderTexture ditherPass = RenderTexture(width, height);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer.screenBuffer.rbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pixelPass.rbo);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ditherMat.setVec4("texelSize", glm::vec4(1 / width, 1 / height, width, height));

		ditherMat.setFloat("spread", spread);
		ditherMat.setInt("redColorCount", redColorCount);
		ditherMat.setInt("greenColorCount", greenColorCount);
		ditherMat.setInt("blueColorCount", blueColorCount);
		ditherMat.setInt("bayerLevel", bayerLevel);
		glViewport(0, 0, width, height);

		drawUtility::blit(pixelPass, ditherPass, ditherMat);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ditherPass.rbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer.screenBuffer.rbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.screenBuffer.framebuffer);

		ditherPass.deleteRenderTex();
		pixelPass.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderInt((std::string("Height##") + std::to_string(pos)).c_str(), &height, 1.0f, 320.0f);
		ImGui::SliderFloat((std::string("Spread##") + std::to_string(pos)).c_str(), &spread, 0.0f, 1.0f);
		ImGui::SliderInt((std::string("Red Color Count##") + std::to_string(pos)).c_str(), &redColorCount, 1, 32);
		ImGui::SliderInt((std::string("Green Color Count##") + std::to_string(pos)).c_str(), &greenColorCount, 1, 32);
		ImGui::SliderInt((std::string("Blue Color Count##") + std::to_string(pos)).c_str(), &blueColorCount, 1, 32);
		ImGui::SliderInt((std::string("Bayer Level##") + std::to_string(pos)).c_str(), &bayerLevel, 0, 2);
	}
};

#endif