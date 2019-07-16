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
			indices.push_back((i)     * (nLon + 1) + (j));
			indices.push_back((i)     * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j + 1));
			indices.push_back((i + 1) * (nLon + 1) + (j));
			indices.push_back((i)     * (nLon + 1) + (j));
		}
	}

	return Mesh(std::move(vertices), std::move(indices));
}

inline Mesh makeCube() {
	std::vector<Vertex> vertices{
		// Back
		{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f},  {0.0f, 1.0f}},
		// Front
		{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f},  {0.0f, 1.0f}},
		// Left
		{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
		// Right
		{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
		// Bottom
		{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f},  {0.0f, 0.0f}},
		// Top
		{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f},  {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f},  {0.0f, 0.0f}},
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
