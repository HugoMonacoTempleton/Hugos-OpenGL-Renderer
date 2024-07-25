#ifndef MODEL_H
#define MODEL_H

#include <custom/Shader.h>
#include <custom/Mesh.h>
#include <custom/TexUtility.h>

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>

/*--------------------------------------------------------------------*/
/* MODEL: Loads model and texture files and stores associated meshes. */
/*--------------------------------------------------------------------*/
class Model {
public:
	Model(std::string const &path) {
		loadModel(path);
	}

	void Draw(Shader& shader) {
		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

	void DeleteModel(){
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].DeleteMesh();
		for (unsigned int i = 0; i < texturesLoaded.size(); i++)
			glDeleteTextures(1, &texturesLoaded[i].id);
	}

private:
	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;
	std::string directory;

	/* Model loading entry point */
	void loadModel(std::string const &path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		// check scene flags are complete
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	/* Processes each node of the scene and calls processMesh() for each mesh */
	void processNode(aiNode* node, const aiScene* scene) {
		//Process current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		//Process children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	/* Constructs a mesh object */
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//Vertices 
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			//Position
			glm::vec3 position;
			position.x = mesh->mVertices[i].x;
			position.y = mesh->mVertices[i].y;
			position.z = mesh->mVertices[i].z;
			vertex.position = position;

			//Normal
			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;
			vertex.normal = normal;

			//TexCoords
			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.texCoords = vec;
			}
			else {
				vertex.texCoords = glm::vec2(0.0f);
			}
			vertices.push_back(vertex);
		}

		//Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		//Textures
		if (mesh->mMaterialIndex >= 0) {
			// Diffuse maps
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// Specular maps
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}

	/* Loads textures from the object material */
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			
			//Check for duplicate
			bool skip = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(texturesLoaded[j]);
					skip = true;
					break;
				}
			}

			//Load texture
			if (!skip) {
				Texture texture;
				texture.id = texUtility::textureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				texturesLoaded.push_back(texture);
			}
		}
		return textures;
	}
};
#endif
