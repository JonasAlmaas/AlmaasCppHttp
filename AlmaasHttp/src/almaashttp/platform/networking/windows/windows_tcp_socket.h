#pragma once

#ifdef ALMAAS_PLATFORM_WINDOWS

#include <almaashttp/networking/tcp_socket.h>

#include <basetsd.h>

// Forward delectation. Source can be found in WinSock2.h
struct sockaddr_in;

namespace Almaas::Networking {

	enum { BufferSize = 32768 };

	class WindowsTCPSocket : public TCPSocket
	{
	public:
		WindowsTCPSocket(const TCPSocketSpec& spec);
		WindowsTCPSocket(UINT_PTR socket, sockaddr_in* clientSockAddr)
			: m_Socket(socket), m_SockAddr(clientSockAddr) {}

		~WindowsTCPSocket();

		virtual bool Send(const char* data, uint32_t size) override;

		virtual int ReceiveBuf(char* buf, uint32_t bufSize) override;
		virtual std::string Receive() override;
		virtual std::string ReceiveLine() override;
		virtual std::string ReceiveBytes(uint32_t amount) override;

		virtual std::string GetHostIP() const override;
		virtual uint16_t GetPort() const override;

		// Server only methods
		virtual std::shared_ptr<TCPSocket> GetNextClient() override;

	private:
		void receiveBufHandle();

	private:
		UINT_PTR m_Socket = (UINT_PTR)(~0); // Originally: SOCKET m_Socket = INVALID_SOCKET (from WinSock2.h)
		sockaddr_in* m_SockAddr = nullptr; // Originally: sockaddr_in m_SockAddr; (from WinSock2.h)

		char m_ReceiveBuf[BufferSize] = { 0 };
		uint32_t m_ReceiveBufStart = 0;
		uint32_t m_ReceiveBytes = 0;

	};

}

#endif // ALMAAS_PLATFORM_WINDOWS
