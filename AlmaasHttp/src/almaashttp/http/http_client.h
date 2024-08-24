#pragma once

#include "http_data.h"

#include <string>

namespace Almaas::Http {

	/**
	 * A simple HTTP client that can send requests and receive responses.
	 */
	class HttpClient
	{
	public:
		static ResponseData Request(Method vethod, const std::string& url);
		static ResponseData Request(Method vethod, const std::string& url, const std::string& data);

	private:
		static bool validateUrl(const std::string& url);

		static void parseResponseLine(ResponseData& response, const std::string& responseLine);
		static void parseHeader(ResponseData& response, const std::string& headerStr);
	};

}
