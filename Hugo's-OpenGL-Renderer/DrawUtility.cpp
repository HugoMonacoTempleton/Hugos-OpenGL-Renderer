#include <custom/DrawUtility.h>
#include <custom/Model.h>
#include <custom/Shader.h>
#include <custom/RenderTexture.h>
#include <custom/BaseMesh.h>
#include <custom/Material.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <iostream>

/*-----------------------------------------*/
/* DRAW UTILITY: Contains drawing methods. */
/*-----------------------------------------*/
namespace drawUtility {

	/* Draws a given object with an outline */
	void drawObjectOutline(Model object, Shader objectShader, Shader outlineShader, glm::mat4 view, glm::mat4 projection) {
		objectShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		objectShader.setMat4("model", model);
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		object.Draw(objectShader);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		outlineShader.use();
		outlineShader.setFloat("scale", 0.1f);
		outlineShader.setMat4("model", model);
		outlineShader.setMat4("view", view);
		outlineShader.setMat4("projection", projection);
		object.Draw(outlineShader);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	/* Copies a texture from a source RT to a destination RT using a given shader/material */
	void blit(RenderTexture source, RenderTexture dest, Shader& shader) {
		BaseMesh& baseMesh = BaseMesh::getInstance();
		
		glBindFramebuffer(GL_FRAMEBUFFER, dest.framebuffer);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, source.textureColorbuffer);
		glBindVertexArray(baseMesh.screenQuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	void blit(RenderTexture source, RenderTexture dest, Material& mat) {	
		BaseMesh& baseMesh = BaseMesh::getInstance();

		glBindFramebuffer(GL_FRAMEBUFFER, dest.framebuffer);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		mat.setTexture("screenTexture", source.textureColorbuffer);
		mat.use();
		glBindVertexArray(baseMesh.screenQuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}
