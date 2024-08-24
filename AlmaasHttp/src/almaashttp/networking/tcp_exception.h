#pragma once

#include <exception>

namespace Almaas::Networking {

	class TCPException : public std::exception
	{
	public:
		TCPException(const char* const message) : std::exception(message) {}
	};

	class TCPClientDisconnectedException : public TCPException
	{
	public:
		TCPClientDisconnectedException(const char* const message) : TCPException(message) {}
	};

	class TCPReceiveException : public TCPException
	{
	public:
		TCPReceiveException(const char* const message) : TCPException(message) {}
	};

	class TCPConnectException : public TCPException
	{
	public:
		TCPConnectException(const char* const message) : TCPException(message) {}
	};

	class TCPBindException : public TCPException
	{
	public:
		TCPBindException(const char* const message) : TCPException(message) {}
	};

	class TCPListenException : public TCPException
	{
	public:
		TCPListenException(const char* const message) : TCPException(message) {}
	};

}
