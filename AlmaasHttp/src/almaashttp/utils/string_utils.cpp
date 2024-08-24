#include "string_utils.h"

#include <sstream>

namespace Almaas::Utils {

	std::string TrimChar(const std::string& str, char trimChar)
	{
		size_t firstNotOf = str.find_first_not_of(trimChar);
		size_t lastNotOf = str.find_last_not_of(trimChar);

		if (firstNotOf == std::string::npos || lastNotOf == std::string::npos) {
			return "";
		}

		return str.substr(firstNotOf, lastNotOf - firstNotOf + 1);
	}

	std::vector<std::string> Split(const std::string& str, char splitChar)
	{
		std::stringstream ss(str);
		std::vector<std::string> result;

		std::string element;
		while (std::getline(ss, element, splitChar)) {
			result.push_back(element);
		}

		return result;
	}

}
