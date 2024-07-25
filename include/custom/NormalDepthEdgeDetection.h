#ifndef NORMALDEPTHEDGE_H
#define NORMALDEPTHEDGE_H

#include <custom/PostProcessingEffect.h> 
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/GBuffer.h>
#include <custom/RenderTexture.h>
#include <custom/DrawUtility.h>

#include <imgui/imgui.h>

#include <string>

/*-----------------------------------------------------------------------------------------------*/
/* NORMAL DEPTH EDGE DETECTION: Uses the screen normal & depth buffer to draw stylised outlines. */
/*-----------------------------------------------------------------------------------------------*/
class NormalDepthEdgeDetection : public PostProcessingEffect {
private:

	Shader multiplyBlendShader;

	Shader copyShader;

	Shader normalDepthOutlineShader;

	Material normalDepthMat;

	Material multiplyBlendMat;

	// Effect properties
	float depthThreshold;

	float normalThreshold;

public:
	NormalDepthEdgeDetection() :
		multiplyBlendShader{ Shader("Shaders/PostProcess.vert","Shaders/MultiplyBlendShader.frag") },
		copyShader{ Shader("Shaders/PostProcess.vert", "Shaders/CopyShader.frag") }, 
		normalDepthOutlineShader{ Shader("Shaders/PostProcess.vert", "Shaders/NormalDepthOutlineShader.frag")},
		normalDepthMat{ Material(normalDepthOutlineShader) },
		multiplyBlendMat{ Material(multiplyBlendShader) }
	{
		depthThreshold = 1.0;
		normalThreshold = 1.0;
	}

	~NormalDepthEdgeDetection() {
		multiplyBlendShader.deleteShader();
		copyShader.deleteShader();
		normalDepthOutlineShader.deleteShader();
		normalDepthMat.deleteMaterial();
	}
	void Render(GBuffer& FrameBuffer, int SCR_WIDTH, int SCR_HEIGHT) override {
		RenderTexture outline = RenderTexture(SCR_WIDTH, SCR_HEIGHT);
		RenderTexture tmp = RenderTexture(SCR_WIDTH, SCR_HEIGHT);

		normalDepthMat.setTexture("normalDepthTexture", FrameBuffer.normalDepthBuffer);
		normalDepthMat.setFloat("_DepthThreshold", depthThreshold);
		normalDepthMat.setFloat("_NormalThreshold", normalThreshold);

		drawUtility::blit(FrameBuffer.screenBuffer, outline, normalDepthMat);
		drawUtility::blit(FrameBuffer.screenBuffer, tmp, copyShader);
		multiplyBlendMat.setTexture("blendLayer", outline.textureColorbuffer);
		drawUtility::blit(tmp, FrameBuffer.screenBuffer, multiplyBlendMat);

		outline.deleteRenderTex();
		tmp.deleteRenderTex();
	}

	void Menu(int pos) override {
		ImGui::SliderFloat((std::string("Depth Threshold##") + std::to_string(pos)).c_str(), &depthThreshold, 0.0f, 4.0f);
		ImGui::SliderFloat((std::string("Normal Threshold##") + std::to_string(pos)).c_str(), &normalThreshold, 0.0f, 4.0f);
	}
};

#endif
