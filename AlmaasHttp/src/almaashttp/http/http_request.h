#pragma once

#include <almaashttp/networking/tcp_socket.h>
#include "http_data.h"

#include <chrono>

namespace Almaas::Http {

	/**
	 * @brief Represents a HTTP request sent by a client.
	 * Once the request goes out of scope, the response will automaticaly be sent back to the client.
	 */
	class HttpRequest
	{
	public:
		HttpRequest(std::shared_ptr<Networking::TCPSocket> client);
		~HttpRequest();

		void Handle();

		inline const RequestData& GetRequestData() const { return m_Request; }
		inline ResponseData& GetResponseData() { return m_Response; }

	private:
		void parseRequestLine(const std::string& requestLine);
		void parseUri(const std::string& uri);
		void parseHeader(const std::string& headerStr);

	private:
		std::shared_ptr<Networking::TCPSocket> m_Client;

		RequestData m_Request;
		ResponseData m_Response;

		std::chrono::steady_clock::time_point m_StartTimepoint;

	};

}
