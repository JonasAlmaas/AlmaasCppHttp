#include "http_data.h"

#include <sstream>

namespace Almaas::Http {

	std::string RequestData::BuildRequest() const
	{
		std::stringstream ss;

		// Request Line
		ss << MethodToString(Method)
			<< " " << Route
			<< " " << ProtocolVersionToString(ProtocolVersion) << "\r\n";

		// Headers
		for (auto& [key, value] : Headers)
		{
			ss << key << ": " << value << "\r\n";
		}
		ss << "\r\n";

		// Body
		ss << Body;

		return ss.str();
	}

	std::string ResponseData::BuildResponse() const
	{
		std::stringstream ss;

		// Response Line
		ss << ProtocolVersionToString(ProtocolVersion)
			<< " " << (uint16_t)StatusCode
			<< " " << StatusCodeToMessage(StatusCode) << "\r\n";

		// Headers
		for (auto& [key, value] : Headers)
		{
			ss << key << ": " << value << "\r\n";
		}
		ss << "\r\n";

		// Body
		ss << Body;

		return ss.str();
	}

}
