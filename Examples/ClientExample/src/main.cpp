#include <iostream>

#include <almaashttp/http/http_exceptions.h>
#include <almaashttp/http/http_Client.h>
#include <almaashttp/networking/tcp_socket.h>

int main(void)
{
	using namespace Almaas::Http;
	using namespace Almaas::Networking;

	TCP::Init();

	ResponseData response = HttpClient::Request(Method::Get, "http://127.0.0.1:54000/ping");

	std::cout << response.BuildResponse() << std::endl;

	TCP::Shutdown();

	return 0;
}
