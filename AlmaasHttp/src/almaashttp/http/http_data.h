#pragma once

#include "http_status_code.h"
#include "http_method.h"
#include "http_version.h"

#include <string>
#include <unordered_map>

namespace Almaas::Http {

	struct RequestData
	{
		Method Method;
		std::string Route;
		std::unordered_map<std::string, std::string> Queries;
		ProtocolVersion ProtocolVersion;
		std::unordered_map<std::string, std::string> Headers;
		std::string Body;

		std::string BuildRequest() const;
	};

	struct ResponseData
	{
		ProtocolVersion ProtocolVersion;
		StatusCode StatusCode;
		std::unordered_map<std::string, std::string> Headers;
		std::string Body;

		std::string BuildResponse() const;
	};
}
