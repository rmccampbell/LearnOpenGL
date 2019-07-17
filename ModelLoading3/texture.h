#pragma once

#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

class Texture
{
public:
	Texture() {}
	Texture(const std::string& path, bool flip = true);
	explicit operator bool() const { return id != 0; }
	void clear() { id = 0; filename.clear(); }
	friend std::ostream& operator<<(std::ostream& os, const Texture& texture);

	unsigned int id = 0;
	std::string filename;
};

inline Texture::Texture(const std::string& path, bool flip) : filename(path)
{
	// load image, create texture and generate mipmaps
	int width, height, channels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << path << ": " << stbi_failure_reason() << std::endl;
		filename.clear();
		id = 0;
		return;
	}
	GLenum formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
	GLenum format = formats[channels];
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// set swizzle mask for grayscale
	if (channels == 1) {
		GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
	else if (channels == 2) {
		GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	}
}

std::ostream& operator<<(std::ostream& os, const Texture& texture)
{
	os << "Texture { " << texture.id;
	if (texture) os << ": " << texture.filename;
	return os << " }";
}
