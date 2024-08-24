#include "http_version.h"

namespace Almaas::Http {

	extern ProtocolVersion ProtocolVersionFromString(const std::string& verStr)
	{
		if (verStr == "HTTP/1.0") return ProtocolVersion::HTTP_1_0;
		if (verStr == "HTTP/1.1") return ProtocolVersion::HTTP_1_1;

		return ProtocolVersion::HTTP_0_9;
	}

	extern std::string ProtocolVersionToString(ProtocolVersion version)
	{
		switch (version)
		{
		case ProtocolVersion::HTTP_0_9: return "HTTP/0.9";
		case ProtocolVersion::HTTP_1_0: return "HTTP/1.0";
		case ProtocolVersion::HTTP_1_1: return "HTTP/1.1";
		}

		return "";
	}

}
