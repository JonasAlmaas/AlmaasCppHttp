#pragma once

#include <string>

namespace Almaas::Http {

	enum class ProtocolVersion : unsigned char { HTTP_0_9, HTTP_1_0, HTTP_1_1 };

	extern ProtocolVersion ProtocolVersionFromString(const std::string& verStr);
	extern std::string ProtocolVersionToString(ProtocolVersion version);

}
