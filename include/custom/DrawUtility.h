#ifndef DRAWUTILITY_H
#define DRAWUTILITY_H

#include <custom/Model.h>
#include <custom/Shader.h>
#include <custom/Material.h>
#include <custom/RenderTexture.h>

#include <glm/glm.hpp>

/*-----------------------------------------*/
/* DRAW UTILITY: Contains drawing methods. */
/*-----------------------------------------*/
namespace drawUtility {
	/* Draws a given object with an outline */
	void drawObjectOutline(Model object, Shader objectShader, Shader outlineShader, glm::mat4 view, glm::mat4 projection);

	/* Copies a texture from a source RT to a destination RT using a given shader */
	void blit(RenderTexture source, RenderTexture dest, Shader& shader);

	void blit(RenderTexture source, RenderTexture dest, Material& mat);
}

#endif
