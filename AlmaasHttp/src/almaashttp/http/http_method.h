#pragma once

#include <string>

namespace Almaas::Http {

	enum class Method : unsigned char { None = 0, Get, Post, Put, Delete };

	extern Method MethodFromString(const std::string& method);
	extern std::string MethodToString(Method method);

}
