#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "material.h"
#include "shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex>&& vertices,
		std::vector<unsigned int>&& indices,
		const Material* material = nullptr)
		: Mesh("", std::move(vertices), std::move(indices), material) {}
	Mesh(std::string_view name,
		std::vector<Vertex>&& vertices,
		std::vector<unsigned int>&& indices,
		const Material* material = nullptr);
	void Draw(const Shader& shader, bool useMaterial = true) const;
private:
	void setupMesh();

public:
	std::string name;
	const Material* material;
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int vao, vbo, ebo;
};

inline Mesh::Mesh(
	std::string_view name,
	std::vector<Vertex>&& vertices,
	std::vector<unsigned int>&& indices,
	const Material* material
) :
	name(name),
	vertices(std::move(vertices)),
	indices(std::move(indices)),
	material(material)
{
	setupMesh();
}

inline void Mesh::setupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
		vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, Position)));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
	glBindVertexArray(0);
}

inline void Mesh::Draw(const Shader& shader, bool useMaterial) const
{
	if (material && useMaterial)
		material->Apply(shader);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
