#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

unsigned int loadTexture(const char *path, bool flip = true);

class Model
{
public:
	Model(const std::string &path)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
private:
	std::unordered_map<std::string, Texture> texture_cache;
	std::vector<Mesh> meshes;
	fs::path directory;
	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
};

inline void Model::Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

inline void Model::loadModel(const std::string & path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path,
		aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate | aiProcess_PreTransformVertices);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	//directory = path.substr(0, path.find_last_of('/'));
	directory = fs::path(path).parent_path();
	processNode(scene->mRootNode, scene);
}

inline void Model::processNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

inline Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		vertex.Position = { 
			mesh->mVertices[i].x, 
			mesh->mVertices[i].y, 
			mesh->mVertices[i].z,
		};
		vertex.Normal = {
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z,
		};
		if (mesh->HasTextureCoords(0)) {
			vertex.TexCoords = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y,
			};
		}
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseMaps = loadMaterialTextures(
		material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture> specularMaps = loadMaterialTextures(
		material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

inline std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString relpath;
		mat->GetTexture(type, i, &relpath);
		Texture texture;
		auto it = texture_cache.find(relpath.C_Str());
		if (it != texture_cache.end()) {
			texture = it->second;
		}
		else {
			std::string path = (directory / relpath.C_Str()).string();
			texture.id = loadTexture(path.c_str());
			texture.type = typeName;
			texture_cache[relpath.C_Str()] = texture;
		}
		textures.push_back(texture);
	}
	return textures;
}

inline unsigned int loadTexture(const char *path, bool flip) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, channels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		GLenum formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		GLenum format = formats[channels];
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture: " << path << ": " << stbi_failure_reason() << std::endl;
	}
	return textureID;
}
