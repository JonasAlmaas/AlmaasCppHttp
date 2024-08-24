#include "http_client.h"

#include <almaashttp/networking/tcp_socket.h>
#include <almaashttp/networking/tcp_exception.h>
#include <almaashttp/utils/string_utils.h>
#include "http_exceptions.h"

#include <iostream>

namespace Almaas::Http {

	ResponseData HttpClient::Request(Method method, const std::string& url)
	{
		return Request(method, url, "");
	}

	ResponseData HttpClient::Request(Method method, const std::string& url, const std::string& data)
	{
		ResponseData response;

		if (!validateUrl(url)) {
			// TODO: Error handling
			return response;
		}

		size_t protocolSize = url.starts_with("http://") ? 7 : 8;

		size_t routePos = url.find_first_of('/', protocolSize);
		size_t portPos = url.find_first_of(':', protocolSize);

		std::string host = routePos != std::string::npos
			? url.substr(protocolSize, (portPos != std::string::npos ? portPos : routePos) - protocolSize)
			: url.substr(protocolSize, (portPos != std::string::npos ? portPos : url.size()) - protocolSize);

		uint16_t port = portPos != std::string::npos
			? (uint16_t)std::stoi(url.substr(portPos + 1, (routePos != std::string::npos ? routePos : url.size()) - portPos - 1))
			: 80;

		Networking::TCPSocketSpec tcpSpec;
		tcpSpec.Type = Networking::TCPSocketType::Client;
		tcpSpec.Host = host.c_str();
		tcpSpec.Port = port;

		std::shared_ptr<Networking::TCPSocket> client;
		try {
			client = Networking::TCPSocket::Create(tcpSpec);
		} catch (const Networking::TCPConnectException& e) {
			std::cerr << e.what() << std::endl;
			// TODO: Error handling
			return response;
		}

		RequestData request;
		request.Method = method;
		request.ProtocolVersion = ProtocolVersion::HTTP_1_1;

		request.Route = routePos != std::string::npos
			? url.substr(routePos)
			: "/";

		request.Body = data;

		if (!request.Body.empty()) {
			request.Headers["Content-Length"] = std::to_string(data.size());
		}

		std::string requestStr = request.BuildRequest();

		std::cout << MethodToString(request.Method) << " " << url << std::endl;
		if (!client->Send(requestStr.c_str(), (uint32_t)requestStr.size())) {
			std::cerr << "Failed to send request" << std::endl;
			// TODO: Error handling
			return response;
		}

		try {
			// Parse Response Line
			std::string responseLine = client->ReceiveLine();
			parseResponseLine(response, responseLine);

			// Parse Request Headers
			std::string headerLine;
			while ((headerLine = client->ReceiveLine()).empty() == false) {
				parseHeader(response, headerLine);
			}

			// Parse Request Body
			if (response.Headers.contains("Content-Length")) {
				uint32_t bodySize = (uint32_t)std::stoi(response.Headers["Content-Length"]);
				response.Body = client->ReceiveBytes(bodySize);
			}
		} catch (const Networking::TCPException& e) {
			std::cerr << e.what() << std::endl;
			// TODO: Error handling
			throw response;
		}

		return response;
	}

	bool HttpClient::validateUrl(const std::string& url)
	{
		return url.starts_with("http://") || url.starts_with("https://");
	}

	void HttpClient::parseResponseLine(ResponseData& response, const std::string& responseLine)
	{
		size_t protocolSplitPos = responseLine.find_first_of(' ');

		if (protocolSplitPos == std::string::npos) {
			throw HttpException("[BadRequest]: Invalid Response Line");
		}

		response.ProtocolVersion = ProtocolVersionFromString(responseLine.substr(0, protocolSplitPos));

		size_t statusCodeSplitPos = responseLine.find_first_of(' ', protocolSplitPos + 1);

		if (statusCodeSplitPos == std::string::npos) {
			throw HttpException("[BadRequest]: Invalid Response Line");
		}

		std::string statusCodeStr = responseLine.substr(protocolSplitPos + 1, statusCodeSplitPos - protocolSplitPos - 1);
		response.StatusCode = (StatusCode)std::stoi(statusCodeStr);
	}

	void HttpClient::parseHeader(ResponseData& response, const std::string& headerStr)
	{
		size_t splitPos = headerStr.find_first_of(": ");

		std::string headerName = splitPos != std::string::npos
			? headerStr.substr(0, splitPos)
			: headerStr;

		std::string headerValue = splitPos != std::string::npos
			? headerStr.substr(splitPos + 2, headerStr.size() - splitPos - 2)
			: "";

		response.Headers[headerName] = headerValue;
	}

}
