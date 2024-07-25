#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



/*---------------------------------------------------------------------------------------------------------*/
/* SHADER: Contains a reference to a complete shader file and provides methods for setting shader uniforms */
/*---------------------------------------------------------------------------------------------------------*/
class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath) {
		//Retrieve source code from filepath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		std::string vertName = vertexPath;
		std::string fragName = fragmentPath;

		vertName = vertName.substr(vertName.rfind("/") + 1, vertName.length());
		fragName = fragName.substr(fragName.rfind("/") + 1, fragName.length());

		vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vertShaderFile.open(vertexPath);
			
			std::stringstream vertShaderStream;

			vertShaderStream << vertShaderFile.rdbuf();

			vertShaderFile.close();

			vertexCode = vertShaderStream.str();
			
		}
		catch(std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl << "AT_FILE: " << vertName << std::endl;
		}

		try {
			fragShaderFile.open(fragmentPath);

			std::stringstream fragShaderStream;

			fragShaderStream << fragShaderFile.rdbuf();

			fragShaderFile.close();

			fragmentCode = fragShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl << "AT_FILE: " << fragName << std::endl;
		}
		const char* vertShaderCode = vertexCode.c_str();
		const char* fragShaderCode = fragmentCode.c_str();

		//build and compile shaders

		//vertex shader
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertex, 1, &vertShaderCode, NULL);
		glCompileShader(vertex);
		
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl << "AT_FILE: " << vertName << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragment, 1, &fragShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl << "AT_FILE: " << fragName << std::endl;
		}

		//Complete shader program
		ID = glCreateProgram();

		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl << "AT_FILES: " << vertName << " " << fragName << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		// Set UBO block IDs
		unsigned int MatricesUniformBlockID = glGetUniformBlockIndex(ID, "Matrices");
		unsigned int TimeUniformBlockID = glGetUniformBlockIndex(ID, "Time");
		unsigned int CameraScreenUniformBlockID = glGetUniformBlockIndex(ID, "CameraScreen");
		unsigned int LightsUniformBlockID = glGetUniformBlockIndex(ID, "Lights");

		glUniformBlockBinding(ID, MatricesUniformBlockID, 0);
		glUniformBlockBinding(ID, TimeUniformBlockID, 1);
		glUniformBlockBinding(ID, CameraScreenUniformBlockID, 2);
		glUniformBlockBinding(ID, LightsUniformBlockID, 3);
	}

	void use() {
		glUseProgram(ID);
	}

	void setBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	
	void setFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	
	void setVec2(const std::string &name, const glm::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	
	void setVec2(const std::string &name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	
	void setVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	
	void setVec3(const std::string &name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	
	void setVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	
	void setVec4(const std::string &name, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	
	void setMat2(const std::string &name, const glm::mat2 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat3(const std::string &name, const glm::mat3 &mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat4(const std::string &name, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void deleteShader() {
		glDeleteProgram(ID);
	}
};

#endif