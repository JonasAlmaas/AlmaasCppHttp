#include "http_method.h"

namespace Almaas::Http {

	extern Method MethodFromString(const std::string& method)
	{
		if (method == "GET") return Method::Get;
		if (method == "POST") return Method::Post;
		if (method == "PUT") return Method::Put;
		if (method == "DELETE") return Method::Delete;
		return Method::None;
	}

	extern std::string MethodToString(Method method)
	{
		switch (method) {
		case Method::Get: return "GET";
		case Method::Post: return "POST";
		case Method::Put: return "PUT";
		case Method::Delete: return "DELETE";
		default: return "NONE";
		}
	}

}
