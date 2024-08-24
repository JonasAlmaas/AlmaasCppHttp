#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Almaas::Networking {

	enum class TCPSocketType : uint8_t { None, Client, Server };

	struct TCPSocketSpec
	{
		TCPSocketType Type = TCPSocketType::None;
		const char* Host = nullptr; // Should be nullptr for a server
		uint16_t Port = 0;

		uint16_t MaxRetries = 5;
		uint16_t RetryDelay = 1000; // In milliseconds
	};

	class TCPSocket
	{
	public:
		~TCPSocket() = default;

		virtual bool Send(const char* data, uint32_t size) = 0;

		virtual int ReceiveBuf(char* buf, uint32_t bufSize) = 0;
		virtual std::string Receive() = 0;
		virtual std::string ReceiveLine() = 0;
		virtual std::string ReceiveBytes(uint32_t amount) = 0;

		virtual std::string GetHostIP() const = 0;
		virtual uint16_t GetPort() const = 0;

		// Server only methods
		virtual std::shared_ptr<TCPSocket> GetNextClient() = 0;

	public:
		static std::shared_ptr<TCPSocket> Create(const TCPSocketSpec& spec);
	};

	// Used to initialize and shut down WinSock on Windows
	class TCP
	{
	public:
		TCP() = delete;

		static void Init();
		static void Shutdown();

	};

}
