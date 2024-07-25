#ifndef MATERIAL_H
#define MATERIAL_H

#include <custom/Shader.h>
#include <custom/TexUtility.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

/*--------------------------------------------------------------------------------------*/
/* MATERIAL: Contains data for all the uniforms that need to be sent to a given shader. */
/*--------------------------------------------------------------------------------------*/
class Material
{
private:
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, int> ints;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, glm::vec2> vec2s;
	std::unordered_map<std::string, glm::vec3> vec3s;
	std::unordered_map<std::string, glm::vec4> vec4s;
	std::unordered_map<std::string, glm::mat2> mat2s;
	std::unordered_map<std::string, glm::mat3> mat3s;
	std::unordered_map<std::string, glm::mat4> mat4s;
	std::unordered_map<std::string, unsigned int*> textureLoaded;

public:
	Shader& shader;
	Material(Shader& shader) : 
		shader { shader }
	{}

	void setBool(const std::string& name, bool value) {
		bools[name] = value;
	}
	
	void setInt(const std::string& name, int value)  {
		ints[name] = value;
	}
	
	void setFloat(const std::string& name, float value)  {
		floats[name] = value;
	}
	
	void setVec2(const std::string& name, const glm::vec2& value)  {
		vec2s[name] = value;
	}
	
	void setVec2(const std::string& name, float x, float y)  {
		vec2s[name] = glm::vec2(x, y);
	}
	
	void setVec3(const std::string& name, const glm::vec3& value)  {
		vec3s[name] = value;
	}
	
	void setVec3(const std::string& name, float x, float y, float z)  {
		vec3s[name] = glm::vec3(x, y, z);
	}
	
	void setVec4(const std::string& name, const glm::vec4& value)  {
		vec4s[name] = value;
	}
	
	void setVec4(const std::string& name, float x, float y, float z, float w)  {
		vec4s[name] = glm::vec4(x, y, z, w);
	}
	
	void setMat2(const std::string& name, const glm::mat2& mat)  {
		mat2s[name] = mat;
	}
	
	void setMat3(const std::string& name, const glm::mat3& mat)  {
		mat3s[name] = mat;
	}

	void setMat4(const std::string& name, const glm::mat4& mat)  {
		mat4s[name] = mat;
	}

	void setTexture(const std::string& name, const char* value) {
		unsigned int tex = texUtility::loadTexture(value);
		textureLoaded[name] = &tex;
	}

	void setTexture(const std::string& name, unsigned int& value) {
		textureLoaded[name] = &value;
	}

	/* Uses the materials shader and passes the relevant uniforms */
	void use() {
		shader.use();
		
		for (auto i = bools.begin(); i != bools.end(); i++) {
			shader.setBool(i->first, i->second);
		}

		for (auto i = ints.begin(); i != ints.end(); i++) {
			shader.setInt(i->first, i->second);
		}

		for (auto i = floats.begin(); i != floats.end(); i++) {
			shader.setFloat(i->first, i->second);
		}

		for (auto i = vec2s.begin(); i != vec2s.end(); i++) {
			shader.setVec2(i->first, i->second);
		}

		for (auto i = vec3s.begin(); i != vec3s.end(); i++) {
			shader.setVec3(i->first, i->second);
		}

		for (auto i = vec4s.begin(); i != vec4s.end(); i++) {
			shader.setVec4(i->first, i->second);
		}

		for (auto i = mat2s.begin(); i != mat2s.end(); i++) {
			shader.setMat2(i->first, i->second);
		}

		for (auto i = mat3s.begin(); i != mat3s.end(); i++) {
			shader.setMat3(i->first, i->second);
		}

		for (auto i = mat4s.begin(); i != mat4s.end(); i++) {
			shader.setMat4(i->first, i->second);
		}

		int texCount = 0;

		for (auto i = textureLoaded.begin(); i != textureLoaded.end(); i++) {
			glActiveTexture(GL_TEXTURE0 + texCount);
			shader.setInt(i->first, texCount++);
			glBindTexture(GL_TEXTURE_2D, *i->second);
		}
	}

	void deleteMaterial() {
		for (auto i = textureLoaded.begin(); i != textureLoaded.end(); i++) {
			glDeleteTextures(1, i->second);
		}
	}
};

#endif