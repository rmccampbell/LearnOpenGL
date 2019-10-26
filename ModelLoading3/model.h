#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "u8tils.h"

namespace fs = std::filesystem;

constexpr int DEFAULT_FLAGS =
	aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
	aiProcess_Triangulate | aiProcess_PreTransformVertices;

class Model
{
public:
	Model(const fs::path& path, bool forceSmooth = false,
		unsigned int flags = DEFAULT_FLAGS);
	void draw(const Shader& shader, bool useMaterial = true) const;
	Material* getMaterial(std::string_view name);
	Mesh* getMesh(std::string_view name);
private:
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh);

public:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

inline Model::Model(const fs::path& path, bool forceSmooth, unsigned int flags)
{
	Assimp::Importer importer;
	if (forceSmooth) {
		flags &= ~aiProcess_GenNormals;
		flags |= aiProcess_GenSmoothNormals | aiProcess_RemoveComponent;
		importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS);
		importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 175.0f);
	}
	const aiScene* scene = importer.ReadFile(u8::path_to_char(path), flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	fs::path directory = path.parent_path();
	materials.reserve(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		materials.emplace_back(scene->mMaterials[i], directory);
	}
	processNode(scene->mRootNode, scene);
}

inline void Model::draw(const Shader& shader, bool useMaterial) const
{
	for (auto& mesh : meshes)
		mesh.draw(shader, useMaterial);
}

inline Material* Model::getMaterial(std::string_view name)
{
	for (auto& mat : materials)
		if (mat.name == name)
			return &mat;
	return nullptr;
}

inline Mesh* Model::getMesh(std::string_view name)
{
	for (auto& mesh : meshes)
		if (mesh.name == name)
			return &mesh;
	return nullptr;
}

inline void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		processMesh(scene->mMeshes[node->mMeshes[i]]);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

inline void Model::processMesh(aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		aiVector3D& v = mesh->mVertices[i];
		vertex.position = { v.x, v.y, v.z };
		aiVector3D& n = mesh->mNormals[i];
		vertex.normal = { n.x, n.y, n.z };
		if (mesh->HasTextureCoords(0)) {
			aiVector3D& tc = mesh->mTextureCoords[0][i];
			vertex.texCoords = { tc.x, tc.y };
		}
		vertices.push_back(vertex);
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	const char* name = mesh->mName.C_Str();
	const Material& material = materials[mesh->mMaterialIndex];

	meshes.emplace_back(name, std::move(vertices), std::move(indices), &material);
}
