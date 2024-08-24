#include <iostream>

#include <almaashttp/http/http_exceptions.h>
#include <almaashttp/http/http_request.h>
#include <almaashttp/networking/tcp_socket.h>

int main(void)
{
	using namespace Almaas;
	using namespace Almaas::Networking;

	TCP::Init();

	TCPSocketSpec spec;
	spec.Type = TCPSocketType::Server;
	spec.Port = 80;

	std::shared_ptr<TCPSocket> httpServer = TCPSocket::Create(spec);

	while (true) {
		// Waits for a client to connect
		std::shared_ptr<TCPSocket> client = httpServer->GetNextClient();
		if (client == nullptr) {
			continue;
		}

		std::cout << "Client connected from "
			<< client->GetHostIP()
			<< " on port "
			<< client->GetPort()
			<< std::endl;

		std::unique_ptr<Http::HttpRequest> request;
		try { request = std::make_unique<Http::HttpRequest>(client); }
		catch (const Http::HttpException&) { break; }

		request->Handle();

		const Http::RequestData& rd = request->GetRequestData();
		std::cout << Http::MethodToString(rd.Method)
			<< " " << rd.Route
			<< " " << Http::ProtocolVersionToString(rd.ProtocolVersion)
			<< " " << rd.Body
			<< std::endl;
	}

	TCP::Shutdown();

	return 0;
}
