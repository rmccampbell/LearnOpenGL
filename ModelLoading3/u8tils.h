#pragma once

#include <string>
#include <filesystem>

namespace utf8 {

inline std::u8string to_u8string(std::string_view s) {
	return std::u8string(s.begin(), s.end());
}

inline std::string to_string(std::u8string_view s) {
	return std::string(s.begin(), s.end());
}

inline std::string_view to_string_view(std::u8string_view s) {
	return std::string_view(reinterpret_cast<const char*>(s.data()), s.size());
}

inline const char* to_char(const char8_t * s) {
	return reinterpret_cast<const char*>(s);
}

inline char* to_char(char8_t * s) {
	return reinterpret_cast<char*>(s);
}

inline const char* to_char(const std::u8string& s) {
	return reinterpret_cast<const char*>(s.c_str());
}

inline std::filesystem::path to_path(std::string_view s) {
	return to_u8string(s);
}

inline std::string path_to_string(const std::filesystem::path & p) {
	return to_string(p.u8string());
}

class _path_to_char {
public:
	_path_to_char(const std::filesystem::path& p) : s8(p.u8string()) {}
	operator const char*() {
		return to_char(s8);
	}
private:
	std::u8string s8;
};

inline _path_to_char path_to_char(const std::filesystem::path & p) {
	return _path_to_char(p);
}

}
