#include <glad/glad.h>

#include <stb_image.h>

#include <iostream>
#include <string>
#include <vector>

/*----------------------------------------------------*/
/* TEXTURE UTILITY: Contains texture loading methods. */
/*----------------------------------------------------*/
namespace texUtility {

	/* Loads a texture and returns an openGL object ID */
	unsigned int loadTexture(char const* path) {
		//Generate texture buffer
		unsigned int textureID;
		glGenTextures(1, &textureID);

		//Load texture data
		stbi_set_flip_vertically_on_load(false);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

		if (data) {
			//Check format
			GLenum format = GL_RGBA;
			if (nrChannels == 1) {
				format = GL_RED;
			}
			else if (nrChannels == 3) {
				format = GL_RGB;
			}
			else if (nrChannels == 4) {
				format = GL_RGBA;
			}

			//Pass data to texture buffer
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//Set texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		return textureID;
	}

	/* Loads a texture with a given directory and path */
	unsigned int textureFromFile(const char* path, const std::string& directory) {
		//Combine path and filename
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		return loadTexture(filename.c_str());
	}

	/* loads a cubemap texture */
	unsigned int loadCubemap(std::vector<std::string> faces) {
		//Generate texture buffer
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		//Bind data for each face
		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else {
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}

		//Set texture parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return textureID;
	}

	/* Loads a cubemap given a directory */
	unsigned int cubemapFromFile(const std::string& directory) {
		//Create faces array
		std::vector<std::string> faces = {
			directory + "/right.jpg",
			directory + "/left.jpg",
			directory + "/top.jpg",
			directory + "/bottom.jpg",
			directory + "/front.jpg",
			directory + "/back.jpg"
		};

		return loadCubemap(faces);
	}
}
