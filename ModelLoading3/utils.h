#pragma once
#include <array>
#include <glad/glad.h>

namespace util {

namespace detail {
void glGetImpl(GLenum pname, GLint* data) {
	glGetIntegerv(pname, data);
}

void glGetImpl(GLenum pname, GLint64* data) {
	glGetInteger64v(pname, data);
}

void glGetImpl(GLenum pname, GLfloat* data) {
	glGetFloatv(pname, data);
}

void glGetImpl(GLenum pname, GLdouble* data) {
	glGetDoublev(pname, data);
}

void glGetImpl(GLenum pname, GLboolean* data) {
	glGetBooleanv(pname, data);
}
}

template <typename T, size_t N=0>
auto glGet(GLenum pname) {
	if constexpr (N == 0) {
		T data;
		detail::glGetImpl(pname, &data);
		return data;
	}
	else {
		std::array<T, N> data;
		detail::glGetImpl(pname, data.data());
		return data;
	}
}

}
