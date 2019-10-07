#pragma once

#include <glm/glm.hpp>
#include "shader.h"

struct DirLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void apply(const Shader& shader, const std::string& name);
};

struct PointLight {
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void apply(const Shader& shader, const std::string& name);
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void apply(const Shader& shader, const std::string& name);
};

void DirLight::apply(const Shader& shader, const std::string& name) {
	shader.setVec3(name + ".direction", direction);
	shader.setVec3(name + ".ambient", ambient);
	shader.setVec3(name + ".diffuse", diffuse);
	shader.setVec3(name + ".specular", specular);
}

void PointLight::apply(const Shader& shader, const std::string& name) {
	shader.setVec3(name + ".position", position);
	shader.setFloat(name + ".constant", constant);
	shader.setFloat(name + ".linear", linear);
	shader.setFloat(name + ".quadratic", quadratic);
	shader.setVec3(name + ".ambient", ambient);
	shader.setVec3(name + ".diffuse", diffuse);
	shader.setVec3(name + ".specular", specular);
}

void SpotLight::apply(const Shader& shader, const std::string& name) {
	shader.setVec3(name + ".position", position);
	shader.setVec3(name + ".direction", direction);
	shader.setFloat(name + ".innerCutoff", innerCutoff);
	shader.setFloat(name + ".outerCutoff", outerCutoff);
	shader.setFloat(name + ".constant", constant);
	shader.setFloat(name + ".linear", linear);
	shader.setFloat(name + ".quadratic", quadratic);
	shader.setVec3(name + ".ambient", ambient);
	shader.setVec3(name + ".diffuse", diffuse);
	shader.setVec3(name + ".specular", specular);
}

