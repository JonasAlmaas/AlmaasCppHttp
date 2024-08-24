#pragma once

#include <string>
#include <vector>

namespace Almaas::Utils {

	std::string TrimChar(const std::string& str, char trimChar);

	std::vector<std::string> Split(const std::string& str, char splitChar);

}
