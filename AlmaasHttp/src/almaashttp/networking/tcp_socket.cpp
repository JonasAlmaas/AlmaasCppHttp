#include "tcp_socket.h"

#include <assert.h>

#ifdef ALMAAS_PLATFORM_WINDOWS
#include <almaashttp/platform/networking/windows/windows_tcp_socket.h>
#endif // ALMAAS_PLATFORM_WINDOWS

namespace Almaas::Networking {

	std::shared_ptr<TCPSocket> TCPSocket::Create(const TCPSocketSpec& spec)
	{
#ifdef ALMAAS_PLATFORM_WINDOWS
		return std::make_shared<WindowsTCPSocket>(spec);
#endif // ALMAAS_PLATFORM_WINDOWS

		assert(false); // Unsupported platform
	}

}
