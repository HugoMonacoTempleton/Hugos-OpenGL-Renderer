#ifndef SOBEL_H
#define SOBEL_H

#include <custom/PostProcessingEffect.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*--------------------------------------------------------------------------------------------------*/
/* SOBEL EDGE DETECTION: Uses a thresholded sobel operator to draw stylised outlines to the screen. */
/*--------------------------------------------------------------------------------------------------*/
class SobelEdgeDetection : public PostProcessingEffect {
private:
	Shader sobelOutlineShader;

	Shader boxBlurShader;

	Shader multiplyBlendShader;

	Shader copyShader;

	Shader grayscaleShader;
	
	Material sobelMat;

	Material boxMat;

	Material multiplyMat;

	// Effect properties
	float threshold;

public:
	SobelEdgeDetection() : 
		sobelOutlineShader{ Shader("Shaders/PostProcess.vert", "Shaders/SobelOutlineShader.frag") },
		boxBlurShader{ Shader("Shaders/PostProcess.vert", "Shaders/BoxBlurShader.frag") },
		multiplyBlendShader{ Shader("Shaders/PostProcess.vert","Shaders/MultiplyBlendShader.frag") },
		copyShader{ Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") } ,
		grayscaleShader { Shader("Shaders/PostProcess.vert", "Shaders/grayscaleShader.frag") },
		sobelMat{ Material(sobelOutlineShader) },
		boxMat { Material(boxBlurShader) },
		multiplyMat { Material(multiplyBlendShader) }
	{
		threshold = 1.0f;
	}

	~SobelEdgeDetection() {
		sobelOutlineShader.deleteShader();
		boxBlurShader.deleteShader();
		multiplyBlendShader.deleteShader();
		copyShader.deleteShader();
		grayscaleShader.deleteShader();
		sobelMat.deleteMaterial();
		boxMat.deleteMaterial();
		multiplyMat.deleteMaterial();
	}

	

	void Render(GBuffer& FrameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture sobelPass1(SCR_WIDTH, SCR_HEIGHT);
		RenderTexture sobelPass2(SCR_WIDTH, SCR_HEIGHT);
		RenderTexture baseLayer(SCR_WIDTH, SCR_HEIGHT);

		sobelMat.setFloat("_Threshold", threshold);

		drawUtility::blit(FrameBuffer.screenBuffer, sobelPass1, boxMat);

		drawUtility::blit(sobelPass1, sobelPass2, grayscaleShader);

		drawUtility::blit(sobelPass2, sobelPass1, sobelMat);

		drawUtility::blit(FrameBuffer.screenBuffer, baseLayer, copyShader);

		multiplyMat.setTexture("blendLayer", sobelPass1.textureColorbuffer);

		drawUtility::blit(baseLayer, FrameBuffer.screenBuffer, multiplyMat);
		
		sobelPass1.deleteRenderTex();
		sobelPass2.deleteRenderTex();
		baseLayer.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderFloat((std::string("Threshold##") + std::to_string(pos)).c_str(), &threshold, 0.0f, 4.0f);
	}
};

#endif
