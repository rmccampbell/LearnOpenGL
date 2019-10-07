#pragma once

#include <iostream>
#include <string>
#include <filesystem>

#include <assimp/material.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "u8tils.h"

namespace fs = std::filesystem;

std::ostream& operator<<(std::ostream& os, const glm::vec2& vec);
std::ostream& operator<<(std::ostream& os, const glm::vec3& vec);
std::ostream& operator<<(std::ostream& os, const glm::vec4& vec);

class Material {
public:
	explicit Material(std::string_view name = "") : name(name) {}
	Material(aiMaterial* mat, const fs::path& directory);
	void apply(const Shader& shader) const;
	friend std::ostream& operator<<(std::ostream& os, const Material& mat);
private:
	static void getColor(aiMaterial* mat, const char* pKey,
		unsigned int type, unsigned int index, glm::vec3& out);
	static Texture getTexture(aiMaterial* mat, const fs::path& directory,
		aiTextureType type, unsigned int index = 0);
public:
	// Material properties
	std::string name;
	float shininess = 0.0f;
	glm::vec3 diffuse_color = { 1.0f, 1.0f, 1.0f };
	glm::vec3 specular_color = { 0.0f, 0.0f, 0.0f };
	glm::vec3 ambient_color = { 1.0f, 1.0f, 1.0f };
	glm::vec3 emissive_color = { 0.0f, 0.0f, 0.0f };
	Texture diffuse_texture;
	Texture specular_texture;
	Texture ambient_texture;
	Texture emissive_texture;
	Texture ao_texture;
	Texture normal_texture;
};


inline Material::Material(aiMaterial* mat, const fs::path& directory) {
	aiString aiName;
	mat->Get(AI_MATKEY_NAME, aiName);
	name = aiName.C_Str();

	mat->Get(AI_MATKEY_SHININESS, shininess);

	getColor(mat, AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
	getColor(mat, AI_MATKEY_COLOR_SPECULAR, specular_color);
	ambient_color = diffuse_color;
	getColor(mat, AI_MATKEY_COLOR_AMBIENT, ambient_color);
	getColor(mat, AI_MATKEY_COLOR_EMISSIVE, emissive_color);

	diffuse_texture = getTexture(mat, directory, aiTextureType_DIFFUSE);
	specular_texture = getTexture(mat, directory, aiTextureType_SPECULAR);
	ambient_texture = getTexture(mat, directory, aiTextureType_AMBIENT);
	emissive_texture = getTexture(mat, directory, aiTextureType_EMISSIVE);
	ao_texture = getTexture(mat, directory, aiTextureType_LIGHTMAP);
	normal_texture = getTexture(mat, directory, aiTextureType_NORMALS);
}

inline void Material::apply(const Shader& shader) const {
	shader.setFloat("material.shininess", shininess);
	shader.setVec3("material.diffuse_color", diffuse_color);
	shader.setVec3("material.specular_color", specular_color);
	shader.setVec3("material.ambient_color", ambient_color);
	shader.setVec3("material.emissive_color", emissive_color);

	diffuse_texture.apply(shader, "material.diffuse_texture", 0);
	specular_texture.apply(shader, "material.specular_texture", 1);
	ambient_texture.apply(shader, "material.ambient_texture", 2);
	emissive_texture.apply(shader, "material.emissive_texture", 3);
	ao_texture.apply(shader, "material.ao_texture", 4);
	normal_texture.apply(shader, "material.normal_texture", 5);
}

inline void Material::getColor(aiMaterial* mat, const char* pKey,
		unsigned int type, unsigned int index, glm::vec3& out) {
	aiColor3D color;
	if (mat->Get(pKey, type, index, color) == AI_SUCCESS) {
		out = { color.r, color.g, color.b };
	}
}

inline Texture Material::getTexture(aiMaterial * mat,
		const fs::path& directory, aiTextureType type, unsigned int index) {
	if (mat->GetTextureCount(type) <= index)
		return Texture();
	aiString aiPath;
	mat->GetTexture(type, index, &aiPath);
	fs::path path = directory / utf8::to_path(aiPath.C_Str());
	// Assume ralative filename if wrong path is hard-coded
	if (!fs::exists(path))
		path = directory / path.filename();
	return Texture(path);
}


inline std::ostream& operator<<(std::ostream& os, const glm::vec2& vec) {
	return os << "{ " << vec.x << ", " << vec.y << " }";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	return os << "{ " << vec.x << ", " << vec.y << ", " << vec.z << " }";
}

inline std::ostream& operator<<(std::ostream& os, const glm::vec4& vec) {
	return os << "{ " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " }";
}

inline std::ostream& operator<<(std::ostream& os, const Material& mat) {
	os << "Name: " << mat.name << '\n';
	os << "Shininess: " << mat.shininess << '\n';
	os << "Diffuse color: " << mat.diffuse_color << '\n';
	os << "Specular color: " << mat.specular_color << '\n';
	os << "Ambient color: " << mat.ambient_color << '\n';
	os << "Emissive color: " << mat.emissive_color << '\n';
	os << "Diffuse texture: " << mat.diffuse_texture << '\n';
	os << "Specular texture: " << mat.specular_texture << '\n';
	os << "Ambient texture: " << mat.ambient_texture << '\n';
	os << "Emissive texture: " << mat.emissive_texture << '\n';
	os << "AO texture: " << mat.ao_texture << '\n';
	os << "Normal texture: " << mat.normal_texture << '\n';
	return os;
}
