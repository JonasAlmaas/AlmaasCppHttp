#ifdef ALMAAS_PLATFORM_WINDOWS

#include "windows_tcp_socket.h"

#include <almaashttp/networking/tcp_exception.h>
#include <almaashttp/utils/string_utils.h>

#include <assert.h>
#include <chrono>
#include <iostream>
#include <thread>

#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

namespace Almaas::Networking {

	TCP::TCP()
	{
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);

		int wsOk = WSAStartup(ver, &wsData);
		if (wsOk != 0) {
			std::cerr << "Unable to initialize Winsock" << std::endl;
			assert(false);
			return;
		}
	}

	TCP::~TCP()
	{
		WSACleanup();
	}

	WindowsTCPSocket::WindowsTCPSocket(const TCPSocketSpec& spec)
	{
		// Create a new socket
		m_Socket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_Socket == INVALID_SOCKET) {
			std::cerr << "Unable to create socket " << WSAGetLastError() << std::endl;
			assert(false);
			return;
		}

		m_SockAddr = new sockaddr_in();

		// Set hints
		m_SockAddr->sin_family = AF_INET;
		m_SockAddr->sin_port = htons(spec.Port);
		spec.Host == nullptr
			? m_SockAddr->sin_addr.S_un.S_addr = INADDR_ANY
			: inet_pton(AF_INET, spec.Host, &m_SockAddr->sin_addr);

		switch (spec.Type) {
		case TCPSocketType::Client:
		{
			int retryCount = 0;
			int connResult = SOCKET_ERROR;
			while (connResult == SOCKET_ERROR && retryCount < spec.MaxRetries) {
				connResult = connect(m_Socket, (sockaddr *)m_SockAddr, sizeof *m_SockAddr);
				if (connResult == SOCKET_ERROR) {
					std::cout << "Unable to connect to server "
						<< spec.Host
						<< " on port "
						<< spec.Port
						<< ". Retrying in "
						<< spec.RetryDelay
						<< "ms"
						<< std::endl;

					std::this_thread::sleep_for(std::chrono::milliseconds(spec.RetryDelay));
					retryCount++;
				}
			}

			if (connResult == SOCKET_ERROR) {
				std::cerr << "Failed to connect to server "
					<< spec.Host
					<< " on port "
					<< spec.Port
					<< " with error "
					<< WSAGetLastError()
					<< std::endl;
				assert(false);
				throw TCPConnectException("Unable to connect");
			}

			break;
		}
		case TCPSocketType::Server:
		{
			// Bind an ip address and port to the socket
			int bindOk = bind(m_Socket, (sockaddr *)m_SockAddr, sizeof *m_SockAddr);
			if (bindOk == SOCKET_ERROR) {
				std::cerr << "Unable to bind socket " << WSAGetLastError() << std::endl;
				assert(false);
				throw TCPBindException("Unable to bind socket");
			}

			// Tell Winsock that the socket is for listening
			int listenOk = listen(m_Socket, SOMAXCONN);

			if (listenOk == SOCKET_ERROR) {
				std::cerr << "Unable to set socket to listen " << WSAGetLastError() << std::endl;
				assert(false);
				throw TCPListenException("Unable to set socket to listen");
			}

			break;
		}
		default:
			std::cerr << "Unknown socket type" << std::endl;
			assert(false);
			break;
		}
	}

	WindowsTCPSocket::~WindowsTCPSocket()
	{
		if (m_Socket != INVALID_SOCKET) {
			shutdown(m_Socket, SD_BOTH);
			closesocket(m_Socket);
		}
	}

	bool WindowsTCPSocket::Send(const char *data, uint32_t size)
	{
		int bytesSent = 0;
		while ((uint32_t)bytesSent < size) {
			int result = send(m_Socket, data, size, 0);
			if (result == SOCKET_ERROR) {
				std::cerr << "Failed to send data to client: " << WSAGetLastError() << std::endl;
				return false;
			}

			bytesSent += result;
		}

		return true;
	}

	int WindowsTCPSocket::ReceiveBuf(char *buf, uint32_t bufSize)
	{
		struct timeval tv = { .tv_sec = 5 };
		fd_set sockets = {0};

		FD_ZERO(&sockets);
		FD_SET(m_Socket, &sockets);

		if (select((int)m_Socket + 1, &sockets, nullptr, nullptr, &tv) <= 0) {
			throw TCPClientDisconnectedException("Timeout in select()");
		}

		int bytesReceived = recv(m_Socket, buf, bufSize, 0);

		if (bytesReceived == SOCKET_ERROR) {
			throw TCPReceiveException("Error in recv()");
		}

		if (bytesReceived == 0) {
			throw TCPClientDisconnectedException("Client disconnected");
		}

		return bytesReceived;
	}

	std::string WindowsTCPSocket::Receive()
	{
		int bytesReceived = ReceiveBuf(m_ReceiveBuf, BufferSize);
		return std::string(m_ReceiveBuf, 0, bytesReceived);
	}

	std::string WindowsTCPSocket::ReceiveLine()
	{
		while (m_ReceiveBytes < BufferSize) {
			std::string data = std::string(m_ReceiveBuf, m_ReceiveBufStart, m_ReceiveBytes - m_ReceiveBufStart);

			size_t eol = data.find_first_of('\n');
			if (eol == std::string::npos) {
				m_ReceiveBytes += ReceiveBuf(m_ReceiveBuf + m_ReceiveBytes, BufferSize - m_ReceiveBytes);
			} else {
				m_ReceiveBufStart += (uint32_t)(eol + 1);
				receiveBufHandle();

				return Utils::TrimChar(data.substr(0, eol), '\r');
			}
		}

		throw TCPReceiveException("Receive too much data");
	}

	std::string WindowsTCPSocket::ReceiveBytes(uint32_t amount)
	{
		while (m_ReceiveBytes < BufferSize) {
			if (amount <= m_ReceiveBytes - m_ReceiveBufStart) {
				std::string data = std::string(m_ReceiveBuf, m_ReceiveBufStart, amount);

				m_ReceiveBufStart += amount;
				receiveBufHandle();

				return data;
			} else {
				m_ReceiveBytes += ReceiveBuf(m_ReceiveBuf + m_ReceiveBytes, BufferSize - m_ReceiveBytes);
			}
		}

		throw TCPReceiveException("Receive too much data");
	}

	std::string WindowsTCPSocket::GetHostIP() const
	{
		char host[NI_MAXHOST];
		std::memset(host, 0, NI_MAXHOST);
		inet_ntop(AF_INET, &m_SockAddr->sin_addr, host, NI_MAXHOST);
		return std::string(host);
	}

	uint16_t WindowsTCPSocket::GetPort() const
	{
		return ntohs(m_SockAddr->sin_port);
	}

	std::shared_ptr<TCPSocket> WindowsTCPSocket::GetNextClient()
	{
		sockaddr_in *clientAddr = new sockaddr_in();
		int clientSize = sizeof *clientAddr;

		SOCKET clientSocket = accept(m_Socket, (sockaddr *)clientAddr, &clientSize);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Unable to create client socket" << std::endl;
			assert(false);
			return nullptr;
		}

		return std::make_shared<WindowsTCPSocket>(clientSocket, clientAddr);
	}

	void WindowsTCPSocket::receiveBufHandle()
	{
		if (m_ReceiveBufStart >= m_ReceiveBytes) {
			m_ReceiveBytes = 0;
			m_ReceiveBufStart = 0;
		}
	}

}

#endif // ALMAAS_PLATFORM_WINDOWS
