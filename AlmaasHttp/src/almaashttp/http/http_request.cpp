#include "http_request.h"

#include <almaashttp/utils/string_utils.h>
#include <almaashttp/networking/tcp_exception.h>
#include "http_exceptions.h"

#include <stdexcept>
#include <iostream>

namespace Almaas::Http {

	HttpRequest::HttpRequest(std::shared_ptr<Networking::TCPSocket> client)
		: m_Client(client)
	{
		m_StartTimepoint = std::chrono::steady_clock::now();
		m_Response.ProtocolVersion = ProtocolVersion::HTTP_1_1;
	}

	void HttpRequest::Handle()
	{
		try {
			// Parse Request Line
			std::string requestLine = m_Client->ReceiveLine();
			if (requestLine.empty()) {
				m_Response.StatusCode = StatusCode::BadRequest;
				throw HttpException("[BadRequest]: No Request Content");
			}

			parseRequestLine(requestLine);

			if (m_Request.ProtocolVersion == ProtocolVersion::HTTP_0_9) {
				if (m_Request.Method != Method::Get) {
					m_Response.StatusCode = StatusCode::BadRequest;
					throw HttpException("[BadRequest]: Only get requests are supported with HTTP/0.9");
				}

				return;
			}

			// Parse Request Headers
			std::string headerLine;
			while ((headerLine = m_Client->ReceiveLine()).empty() == false) {
				parseHeader(headerLine);
			}

			// Parse Request Body
			if (m_Request.Headers.contains("Content-Length")) {
				uint32_t bodySize;

				try {
					bodySize = (uint32_t)std::stoi(m_Request.Headers["Content-Length"]);
				} catch (...) {
					bodySize = 0;
					std::cout << "Invalid value \""
						<< m_Request.Headers["Content-Length"]
						<< "\" for header \"Content-Length\""
						<< std::endl;

					m_Response.StatusCode = StatusCode::BadRequest;
					throw HttpException("[BadRequest]: Invalid header value for \"Content-Length\"");
				}

				m_Request.Body = m_Client->ReceiveBytes(bodySize);
			}
		} catch (const Networking::TCPException&) {
			m_Client = nullptr;
			throw HttpException("Client disconnected");
		}
	}

	HttpRequest::~HttpRequest()
	{
		if (m_Client == nullptr) {
			std::cout << "Client disconnected" << std::endl;
			return;
		}

		if (!m_Response.Body.empty()) {
			m_Response.Headers["Content-Length"] = std::to_string(m_Response.Body.size());
		}

		std::string responseStr = m_Response.BuildResponse();

		std::chrono::steady_clock::time_point endTimepoint = std::chrono::steady_clock::now();
		std::chrono::duration epochStart = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimepoint).time_since_epoch();
		std::chrono::duration epochEnd = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimepoint).time_since_epoch();
		std::chrono::duration elapsedTime = epochStart - epochEnd;

		std::cout << MethodToString(m_Request.Method)
			<< " "
			<< m_Request.Route
			<< " finished in "
			<< elapsedTime
			<< " with status "
			<< (int)m_Response.StatusCode
			<< " "
			<< StatusCodeToMessage(m_Response.StatusCode)
			<< std::endl;

		if (!m_Client->Send(responseStr.c_str(), (uint32_t)responseStr.size())) {
			std::cerr << "Failed to send response to client" << std::endl;
		}
	}

	void HttpRequest::parseRequestLine(const std::string& requestLine)
	{
		size_t methodSplitPos = requestLine.find_first_of(' ');

		if (methodSplitPos == std::string::npos) {
			m_Response.StatusCode = StatusCode::BadRequest;
			throw HttpException("[BadRequest]: Invalid Request Line");
		}

		std::string methodStr = requestLine.substr(0, methodSplitPos);
		m_Request.Method = MethodFromString(methodStr);

		if (m_Request.Method == Method::None) {
			m_Response.StatusCode = StatusCode::MethodNotAllowed;
			throw HttpException("[MethodNotAllowed]: Method Not Allowed");
		}

		size_t versionSplitPos = requestLine.find_last_of(' ');

		std::string uriStr = versionSplitPos != std::string::npos
			? requestLine.substr(methodSplitPos + 1, versionSplitPos - methodSplitPos - 1)
			: requestLine.substr(methodSplitPos + 1, requestLine.size() - methodSplitPos - 1);

		parseUri(uriStr);

		std::string protocolVersionStr = versionSplitPos != std::string::npos
			? requestLine.substr(versionSplitPos + 1, requestLine.size() - versionSplitPos)
			: "";

		m_Request.ProtocolVersion = ProtocolVersionFromString(protocolVersionStr);
	}

	void HttpRequest::parseUri(const std::string& uri)
	{
		size_t querySplitPos = uri.find_first_of('?');
		m_Request.Route = uri.substr(0, querySplitPos);

		if (querySplitPos == std::string::npos) {
			return;
		}

		// Parse query parameters
		size_t queryPos = querySplitPos;

		while (queryPos != std::string::npos) {
			++queryPos;

			size_t nextPos = uri.find_first_of('&', queryPos);

			std::string queryStr = nextPos != std::string::npos
				? uri.substr(queryPos, nextPos - queryPos)
				: uri.substr(queryPos, uri.size() - queryPos);

			size_t equalPos = queryStr.find_first_of('=');

			std::string key = equalPos != std::string::npos
				? queryStr.substr(0, equalPos)
				: queryStr;

			std::string value = equalPos != std::string::npos
				? queryStr.substr(equalPos + 1, queryStr.size() - equalPos - 1)
				: "";

			m_Request.Queries[key] = value;
			queryPos = nextPos;
		}
	}

	void HttpRequest::parseHeader(const std::string& headerStr)
	{
		size_t splitPos = headerStr.find_first_of(": ");

		std::string headerName = splitPos != std::string::npos
			? headerStr.substr(0, splitPos)
			: headerStr;

		std::string headerValue = splitPos != std::string::npos
			? headerStr.substr(splitPos + 2, headerStr.size() - splitPos - 2)
			: "";

		m_Request.Headers[headerName] = headerValue;
	}

}
