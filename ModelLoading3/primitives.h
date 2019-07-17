#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "mesh.h"

inline Mesh makeSphere(unsigned int nLat = 16, unsigned int nLon = 32) {
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < nLat + 1; i++) {
		for (unsigned int j = 0; j < nLon + 1; j++) {
			float theta = float(j) / nLon * glm::two_pi<float>();
			float phi = (float(i) / nLat - 0.5f) * glm::pi<float>();
			float x = glm::cos(theta) * glm::cos(phi);
			float y = glm::sin(phi);
			float z = -glm::sin(theta) * glm::cos(phi);
			float u = float(j) / nLon;
			float v = float(i) / nLat;
			vertices.push_back({ {x, y, z}, {x, y, z}, {u, v} });
		}
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < nLat; i++) {
		for (unsigned int j = 0; j < nLon; j++) {
			indices.push_back((i + 0) * (nLon + 1) + (j + 0));
			indices.push_back((i + 0) * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j + 0));
			indices.push_back((i + 0) * (nLon + 1) + (j + 0));
		}
	}

	return Mesh(std::move(vertices), std::move(indices));
}

inline Mesh makeCube(bool unwrap = false) {
	std::vector<glm::vec2> texCoords;
	if (unwrap) {
		texCoords = {
			{0.75f, 1.0f / 3}, {1.00f, 1.0f / 3}, {1.00f, 2.0f / 3}, {0.75f, 2.0f / 3},
			{0.25f, 1.0f / 3}, {0.50f, 1.0f / 3}, {0.50f, 2.0f / 3}, {0.25f, 2.0f / 3},
			{0.00f, 1.0f / 3}, {0.25f, 1.0f / 3}, {0.25f, 2.0f / 3}, {0.00f, 2.0f / 3},
			{0.50f, 1.0f / 3}, {0.75f, 1.0f / 3}, {0.75f, 2.0f / 3}, {0.50f, 2.0f / 3},
			{0.25f, 0.0f / 3}, {0.50f, 0.0f / 3}, {0.50f, 1.0f / 3}, {0.25f, 1.0f / 3},
			{0.25f, 2.0f / 3}, {0.50f, 2.0f / 3}, {0.50f, 3.0f / 3}, {0.25f, 3.0f / 3},
		};
	}
	else {
		texCoords = {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
		};
	}
	std::vector<Vertex> vertices{
		// Back
		{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, texCoords[0]},
		{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, texCoords[1]},
		{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, texCoords[2]},
		{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, texCoords[3]},
		// Front
		{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, texCoords[4]},
		{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, texCoords[5]},
		{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, texCoords[6]},
		{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, texCoords[7]},
		// Left
		{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, texCoords[8]},
		{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, texCoords[9]},
		{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, texCoords[10]},
		{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, texCoords[11]},
		// Right
		{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, texCoords[12]},
		{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, texCoords[13]},
		{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, texCoords[14]},
		{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, texCoords[15]},
		// Bottom
		{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, texCoords[16]},
		{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, texCoords[17]},
		{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, texCoords[18]},
		{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, texCoords[19]},
		// Top
		{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, texCoords[20]},
		{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, texCoords[21]},
		{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, texCoords[22]},
		{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, texCoords[23]},
	};
	std::vector<unsigned int> indices{
		 0,  1,  2,  2,  3,  0,
		 4,  5,  6,  6,  7,  4,
		 8,  9, 10, 10, 11,  8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20,
	};
	return Mesh(std::move(vertices), std::move(indices));
}
