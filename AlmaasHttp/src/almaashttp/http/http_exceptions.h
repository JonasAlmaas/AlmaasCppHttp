#pragma once

#include <exception>

namespace Almaas::Http {

	class HttpException : public std::exception
	{
	public:
		HttpException(const char *const message) : std::exception(message) {}
	};

}
