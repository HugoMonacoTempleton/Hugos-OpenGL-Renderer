#ifndef TEXUTILITY_H
#define TEXUTILITY_H

#include <glm/glm.hpp>

#include <string>
#include <vector>

/*----------------------------------------------------*/
/* TEXTURE UTILITY: Contains texture loading methods. */
/*----------------------------------------------------*/
namespace texUtility {
	/* Loads a texture and returns an openGL object ID */
	unsigned int loadTexture(char const* path);

	/* Loads a texture with a given directory and path */
	unsigned int textureFromFile(const char* path, const std::string& directory);

	/* loads a cubemap texture */
	unsigned int loadCubemap(std::vector<std::string> faces);

	/* Loads a cubemap given a directory */
	unsigned int cubemapFromFile(const std::string& directory);
}
#endif 
