/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose: Handles TCP connections
	
*********************************************************************/

#include "czPlatformStableHeaders.h"
#include "czTCPSocket.h"
#include "czLogger.h"
#include "czThread.h"

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	// Need to link with Ws2_32.lib
	#pragma comment(lib, "ws2_32.lib")
#endif


namespace cz
{

	//
	// IPAddress
	// 
	bool IPAddress::createFrom(const char* str, IPAddress &dest)
	{
		String s(str);
		TArray<String> parts;
		cz::splitString(str, '.', parts);
		if (parts.size()!=4)
		{
			return false;
		}

		int bytes[4];
		for(int i=0; i<4; i++)
		{
			bytes[i] = atoi(parts[i].c_str());
			if (bytes[i]<0 || bytes[i]>255)
				return false;
		}

		dest = IPAddress(bytes[0],bytes[1],bytes[2],bytes[3]);
		return true;
	}

	IPAddress::IPAddress(const char *str)
	{
		full=0;
		createFrom(str, *this);		
	}

	const char* IPAddress::toString() const
	{
		return  formatString("%d.%d.%d.%d", bytes.b1, bytes.b2, bytes.b3, bytes.b4);
	}


#if CZ_PLATFORM==CZ_PLATFORM_WIN32
	u32 IPAddress::hostToNetworkL(u32 host)
	{
		return htonl(host);
	}
	u16 IPAddress::hostToNetworkS(u16 host)
	{
		return htons(host);
	}
	u32 IPAddress::networkToHostL(u32 network)
	{
		return ntohl(network);
	}
	u16 IPAddress::networkToHostS(u16 network)
	{
		return ntohs(network);
	}
#else
	#error Platform not supported
#endif

	//
	// SocketAddress
	// 
	const char* SocketAddress::toString() const
	{
		return formatString("%s:%d", m_addr.toString(), (int)m_port);
	}

	bool SocketAddress::createFrom(const char *str, SocketAddress &dest)
	{
		String s(str);
		TArray<String> parts;
		cz::splitString(str, ':', parts);
		if (parts.size()!=2)
			return false;

		if (!IPAddress::createFrom(parts[0].c_str(), dest.m_addr))
			return false;

		dest.m_port = (u16)atoi(parts[1].c_str());
		return true;
	}


	//
	// TCPSocketManager
	//

	CZ_DEFINE_SINGLETON(cz::TCPSocketManager)

	TCPSocketManager::TCPSocketManager()
	{
	}

	TCPSocketManager::~TCPSocketManager()
	{

		// TODO: TERRIBLE HACK to shutdown all sockets
		// Remove this shit! Need to refactor sockets to have a clean way to close them
		int loop=10;
		while(loop--)
		{
			TCPSocketManager::tick(0.005f);
			cz::this_thread::sleep_ms(5);
		}
		for(int i=0; i<m_sockets.size(); i++)
		{
			m_sockets[i]->close();
		}
		loop=10;
		while(loop--)
		{
			TCPSocketManager::tick(0.005f);
			cz::this_thread::sleep_ms(5);
		}
		//
		//
		//

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
		WSACleanup( );
#else
	#error Platform not supported
#endif

		for(int i=0; i<m_sockets.size(); i++)
		{
			TCPSocket *psocket = m_sockets[i];
			CZ_DELETE psocket;
		}
	}

	bool TCPSocketManager::initialize()
	{
#if CZ_PLATFORM==CZ_PLATFORM_WIN32

		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 2, 2 );

		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			czError(ID_Network, "Error initializing WinSock");
			return false;
		}

		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
				czError(ID_Network, "Error initializing required WinSock version (2.2)");
				WSACleanup( );
				return false; 
		}

		return true;
#else
	#error Platform not supported
#endif	
	}

	float TCPSocketManager::tick(float deltaseconds)
	{
#if CZ_PLATFORM==CZ_PLATFORM_WIN32

		// Check status for all sockets
		{
			fd_set readfds;
			fd_set writefds;
			fd_set errorfds;
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&errorfds);
			for(int i=0; i<m_sockets.size(); i++)
			{
				FD_SET(m_sockets[i]->m_osdata.getOSSocket(), &readfds);
				FD_SET(m_sockets[i]->m_osdata.getOSSocket(), &writefds);
				FD_SET(m_sockets[i]->m_osdata.getOSSocket(), &errorfds);
			}
			TIMEVAL t = {0,0};
			if (select(0, &readfds, &writefds, &errorfds, &t)!=SOCKET_ERROR)
			{
				for(int i=0; i<m_sockets.size(); i++)
				{
					m_sockets[i]->setStatusFlags(
						FD_ISSET(m_sockets[i]->m_osdata.getOSSocket(), &readfds)!=0 ? true : false,
						FD_ISSET(m_sockets[i]->m_osdata.getOSSocket(), &writefds)!=0 ? true : false,
						FD_ISSET(m_sockets[i]->m_osdata.getOSSocket(), &errorfds)!=0 ? true : false);
				}	
			}
		}
#else
	#error Platform not supported
#endif

		for(int i=0; i<m_sockets.size(); i++)
		{
			m_sockets[i]->tick();
		}

		destroyPending();
		return std::numeric_limits<float>::min();
	}

	void TCPSocketManager::destroyPending()
	{
		for(int i=0; i<m_sockets.size(); i++)
		{
			if (m_sockets[i]->m_pendingDestroy)
			{
				TCPSocket *psocket = m_sockets[i];
				m_sockets.removeAtIndexAndFillWithLast(i);		
				i--; // so we process the same index again
				CZ_DELETE psocket;
			}
		}
	}

	TCPSocket* TCPSocketManager::createSocket()
	{
		TCPSocket *pSocket = CZ_NEW TCPSocket();
		if (!pSocket->initialize())
		{
			CZ_DELETE pSocket;
			return NULL;
		}
		else
		{
			m_sockets.push_back(pSocket);
			return pSocket;		
		}
	}

	void TCPSocketManager::destroySocket(TCPSocket* pSocket)
	{
		pSocket->m_pendingDestroy = true;
	}

	//
	// TCPSocket
	// 

	TCPSocket::TCPSocket()
	{
		m_state = kSOCKET_DISCONNECTED;
		m_pListener = NULL;
		m_bReadyToRead = false;
		m_bReadyToSend = false;
		m_bError = false;
		m_osdata.outer = this;
		m_pendingDestroy = false;
		m_cookie = nullptr;
	}

	TCPSocket::~TCPSocket()
	{
	}

	bool TCPSocket::initialize()
	{
		return (m_osdata.initialize()==kRESULT_OK) ? true : false;
	}

	void TCPSocket::setListener(class TCPSocketListener *pListener)
	{
		m_pListener = pListener;
	}

	bool TCPSocket::close()
	{
		if (m_state==kSOCKET_DISCONNECTED)
			return true;

		if (!(
			m_state==kSOCKET_LISTENING ||
			m_state==kSOCKET_CONNECTING ||
			m_state==kSOCKET_CONNECTED))
		{
			czError(ID_Network, "Socket is in invalid state (%d)", m_state);
			return false;
		}


		if (m_osdata.close()==kRESULT_OK)
		{
			m_state = kSOCKET_DISCONNECTING;
			return true;
		}
		else
		{
			m_state = kSOCKET_DISCONNECTED;
			return false;
		}
	}

	bool TCPSocket::bind(const SocketAddress &addr)
	{
		if (!(
			m_state==kSOCKET_DISCONNECTED
			))
		{
			czError(ID_Network, "Socket is in invalid state (%d)", m_state);
			return false;
		}

		if (m_osdata.bind(addr)==kRESULT_OK)
		{
			m_localAddr = addr;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TCPSocket::listen(int backlog)
	{
		if (!(
			m_state==kSOCKET_DISCONNECTED
			))
		{
			czError(ID_Network, "Socket is in invalid state (%d)", m_state);
			return false;
		}

		if (m_osdata.listen(backlog)==kRESULT_OK)
		{
			m_osdata.getLocalAddress(&m_localAddr);
			m_state = kSOCKET_LISTENING;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TCPSocket::connect(const SocketAddress &addr)
	{
		if (!(
			m_state==kSOCKET_DISCONNECTED
			))
		{
			czError(ID_Network, "Socket is in invalid state (%d)", m_state);
			return false;
		}

		if (m_osdata.connect(addr)==kRESULT_OK)
		{
			m_peerAddr = addr;
			m_state = kSOCKET_CONNECTING;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TCPSocket::send(const void *buf, int size)
	{
		if (!(
			m_state==kSOCKET_CONNECTED
			))
		{
			czError(ID_Network, "Socket is in invalid state (%d)", m_state);
			return false;
		}

		m_sendbuffer.write(buf, size);
		return true;
	}

	void TCPSocket::setStatusFlags(bool bReadable, bool bWritable, bool bError)
	{
		m_bError = bError;
		m_bReadyToRead = bReadable;
		m_bReadyToSend = bWritable;
	}

	bool TCPSocket::doSend()
	{
		// Try to send data
		if (m_sendbuffer.getUsedSize() && m_bReadyToSend)
		{
			void *buf;
			int size;

			// I get the internal pointer, and then "Skip", to avoid copying memory around
			while((size=m_sendbuffer.getReadPointer(&buf))>0)
			{
				int sentsize;
				OperationResult res = m_osdata.send(buf, size, &sentsize);
				if (res==kRESULT_OK)
				{
					m_sendbuffer.skip(sentsize);
					if (sentsize!=size)
						break;
				}
				else
				{
					// If something went wrong, so we need to shutdown
					m_state = kSOCKET_DISCONNECTED;
					if (m_pListener)
						m_pListener->onSocketClosed(*this, kSOCKET_CLOSED_REASON_SYSTEM);
					return false;
				}
			}
		}

		return true;
	}

	bool TCPSocket::doReceive()
	{
		// Try to receive data
		if (m_bReadyToRead)
		{
			const int maxsize=1024*64;
			char buf[maxsize];
			int receivedsize;
			
			// Due the bad design of all this TCP code, if I just keep retrieving data while it's available, it means
			// this code will never return if the other socket is sending data too fast and keeps our side always busy.
			// So, I'm limiting how much can we do before returning
			int receivedTodo = 1024*1024;

			while(1)
			{
				if (receivedTodo<=0) // Enough work done, so bailout
					return true;

				OperationResult res = m_osdata.receive(buf, maxsize, &receivedsize);
				switch(res)
				{
				case kRESULT_OK:
					if (receivedsize)
					{
						receivedTodo -= receivedsize;
						m_recvbuffer.write(buf, receivedsize);
						if (m_pListener)
							m_pListener->onSocketReceived(*this);
					}
					else
					{
						// No more data, so break the loop
						return true;
					}
					break;
				case kRESULT_PEER_SHUTDOWN:
					// Peer socket closed the connection graceful
					m_state = kSOCKET_DISCONNECTED;
					m_osdata.close();
					if (m_pListener)
						m_pListener->onSocketClosed(*this, kSOCKET_CLOSED_REASON_NORMAL);
					return true;
					break;
				case kRESULT_ERROR:
					m_state = kSOCKET_DISCONNECTED;
					if (m_pListener)
						m_pListener->onSocketClosed(*this, kSOCKET_CLOSED_REASON_SYSTEM);
					return false;
					break;
				default:
					CZ_ASSERT(0);
				}
			}
		}

		return true;
	}

	void TCPSocket::tick()
	{
		// Check if the socket got closed meanwhile
		ClosedReason closereason;
		if (m_osdata.checkClosed(&closereason))
		{
			m_osdata.close();
			m_state = kSOCKET_DISCONNECTED;
			if (m_pListener)
				m_pListener->onSocketClosed(*this, closereason);
		}

		switch (m_state)
		{
		case kSOCKET_CONNECTING:
			{
				if (m_bError)
				{
					m_bError = false;
					m_state = kSOCKET_DISCONNECTED;
					if (m_pListener)
						m_pListener->onSocketConnectFailed(*this);
				}
				else if (m_bReadyToSend)
				{
					m_osdata.getLocalAddress(&m_localAddr);
					m_state = kSOCKET_CONNECTED;
					if (m_pListener)
						m_pListener->onSocketConnected(*this);				
				}
			}
			break;
		case kSOCKET_LISTENING:
			{
				if (m_bReadyToRead)
				{
					m_bReadyToRead = false;
					if (m_pListener)
					{
						while(1)
						{
							TCPSocket *pNewPeer;
							OperationResult res = m_osdata.accept(&pNewPeer);
							if (res==kRESULT_OK)
							{
								if (pNewPeer)
								{
									pNewPeer->m_state = kSOCKET_CONNECTED;
									pNewPeer->m_localAddr = m_localAddr;
									TCPSocketManager::getSingleton().m_sockets.push_back(pNewPeer);
									m_pListener->onSocketAccept(*this, pNewPeer);
								}
								else
								{
									// No more pending connections, so break the loop
									break;
								}
							}
							else
							{
								m_state = kSOCKET_DISCONNECTED;
								if (m_pListener)
									m_pListener->onSocketConnectFailed(*this);
								// break the loop
								break;
							}
						}
					}
				}		
			}
			break;
		case kSOCKET_CONNECTED:
			{
				if (doSend()) // DoSend might detect an error, so we only can DoReceive if everything is still ok
					doReceive();
			}
			break;
		case kSOCKET_DISCONNECTING:
			{
				doReceive();
			}
			break;
		case kSOCKET_DISCONNECTED:
			break;
		default:		
			break;
		}

	}


	const char* TCPSocket::getErrorMessage(int err)
	{
	#if CZ_PLATFORM==CZ_PLATFORM_WIN32
		return TCPSocket::Win32Obj::getErrorMessage(err);
	#else
		#error unsupported platform
	#endif
	}

	String& operator<< (String& s, const IPAddress& val)
	{
		s.append(val.toString());
		return s;
	}
	String& operator<< (String& s, const SocketAddress& val)
	{
		s.append(val.toString());
		return s;
	}



#if CZ_PLATFORM==CZ_PLATFORM_WIN32

	/////////////////////////////////////
	//
	// WinSock IMPLEMENTATION
	//
	/////////////////////////////////////

	const char* TCPSocket::Win32Obj::getErrorMessage(int err)
	{
		int errCode = (err==0) ? WSAGetLastError() : err;
		static char errString[512];
		// http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
		int size = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
			0,       // 0 since source is internal message table
			errCode, // this is the error code returned by WSAGetLastError()
			// Could just as well have been an error code from generic
			// Windows errors from GetLastError()
			0,       // auto-determine language to use
			errString, // this is WHERE we want FormatMessage
			_countof(errString),
			0 );               // 0, since getting message from system tables
		return errString;
	}

	TCPSocket::Win32Obj::Win32Obj()
	{
	}

	TCPSocket::Win32Obj::~Win32Obj()
	{
		if (closesocket(m_socket)==SOCKET_ERROR)
		{
			czError(ID_Network, "Error calling closesocket : %s", getErrorMessage());
		}
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::initialize()
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_socket == INVALID_SOCKET)
		{
			czError(ID_Network, "Error initializing socket : %s", getErrorMessage());
			return kRESULT_ERROR;
		}

		//-------------------------
		// Set the socket I/O mode: In this case FIONBIO
		// enables or disables the blocking mode for the 
		// socket based on the numerical value of iMode.
		// If iMode = 0, blocking is enabled; 
		// If iMode != 0, non-blocking mode is enabled.
		//
		int iMode = 1;
		if (ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode) == SOCKET_ERROR)
		{
			czError(ID_Network, "Error setting socket as non-blocking : %s", getErrorMessage());
			return kRESULT_ERROR;
		}

		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::close()
	{
		// From MSDN, the proper way to graceful disconnect a socket is:
		// 1. Call WSAAsyncSelect to register for FD_CLOSE notification. 
		// 2. Call shutdown with how=SD_SEND. 
		// 3. When FD_CLOSE received, call recv until zero returned, or SOCKET_ERROR. 
		// 4. Call closesocket.
		if (shutdown(m_socket, SD_SEND)==SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err==WSAENOTCONN && outer->m_state==kSOCKET_LISTENING)
			{
				return kRESULT_OK;
			}
			else
			{
				czError(ID_Network, "Call to shutdown failed : %s", getErrorMessage(err));
				return kRESULT_ERROR;
			}
		}

		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::bind(const SocketAddress &addr)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.S_un.S_addr = addr.m_addr.full;
		service.sin_port = IPAddress::hostToNetworkS(addr.m_port);
		if (::bind(m_socket, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR)
		{
				czError(ID_Network, "Call to bind failed : %s", getErrorMessage());
				return kRESULT_ERROR;
		}

		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::listen(int backlog)
	{
		if (::listen(m_socket, (backlog==-1) ? SOMAXCONN : backlog)==SOCKET_ERROR)
		{
			czError(ID_Network, "Call to listen failed : %s", getErrorMessage());
			return kRESULT_ERROR;
		}

		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::accept(TCPSocket** ppNewPeer)
	{
		sockaddr_in localinfo;
		int size = sizeof(localinfo);
		SOCKET newpeer = ::accept(m_socket, (SOCKADDR*)&localinfo, &size);
		if (newpeer==SOCKET_ERROR)
		{
			*ppNewPeer = NULL;
			int err = WSAGetLastError();
			if (err==WSAEWOULDBLOCK||err==WSAECONNRESET||err==WSAEMFILE)
			{
				return kRESULT_OK;
			}
			else
			{
				czError(ID_Network, "Call to accept failed : %s", getErrorMessage(err));
				return kRESULT_ERROR;
			}
		}

		TCPSocket *pNewPeer = CZ_NEW TCPSocket;
		pNewPeer->m_osdata.m_socket = newpeer;
		if (size==sizeof(localinfo))
		{
			pNewPeer->m_peerAddr.m_addr.full = localinfo.sin_addr.s_addr;
			pNewPeer->m_peerAddr.m_port = IPAddress::networkToHostS(localinfo.sin_port);
		}
		*ppNewPeer = pNewPeer;
		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::connect(const SocketAddress &addr)
	{
		sockaddr_in clientService;
		memset(&clientService, 0, sizeof(clientService));
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr =  addr.m_addr.full;
		clientService.sin_port = IPAddress::hostToNetworkS(addr.m_port);

		if (::connect(m_socket, (SOCKADDR*) &clientService, sizeof(clientService)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err==WSAEWOULDBLOCK)
			{
				return kRESULT_OK;
			}
			else
			{
				czError(ID_Network, "Call to connect failed : %s", getErrorMessage(err));
				return kRESULT_ERROR;
			}
		}

		return kRESULT_OK;
	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::send(void *buf, int size, int *sentsize)
	{
		int ret = ::send(m_socket, (char*)buf, size, 0);
		if (ret==SOCKET_ERROR)
		{
			*sentsize = 0;
			int err = WSAGetLastError();
			if (err==WSAEWOULDBLOCK)
			{
				outer->m_bReadyToSend = false; // Reset, as it means we can write more data
				return kRESULT_OK;
			}
			else
			{
				czWarning(ID_Network, "call to send failed : %s", getErrorMessage(err));
				return kRESULT_ERROR;
			}
		}
		else
		{
			*sentsize = ret;
			return kRESULT_OK;
		}

	}

	TCPSocket::OperationResult TCPSocket::Win32Obj::receive(void *buf, int maxsize, int *receivedsize)
	{
		// From MSDN :
		// If no error occurs, recv returns the number of bytes received.
		// If the connection has been gracefully closed, the return value is zero.
		// Otherwise, a value of SOCKET_ERROR is returned, and a specific error code can
		// be retrieved by calling WSAGetLastError.

		int ret = recv(m_socket, (char*)buf, maxsize, 0);
		if (ret==SOCKET_ERROR)
		{
			*receivedsize = 0;
			int err = WSAGetLastError();
			// a WSAEWOULDBLOCK is normal, as it means there is no data to read
			if (err==WSAEWOULDBLOCK)
			{
				outer->m_bReadyToRead = false; // Reset. as it means we dont have more data to read
				return kRESULT_OK;
			}
			else
			{
				czWarning(ID_Network, "error calling recv : %s", getErrorMessage(err));
				return kRESULT_ERROR;
			}
		}
		else
		{
			*receivedsize = ret;
			if (ret)
				return kRESULT_OK;
			else
				return kRESULT_PEER_SHUTDOWN;
		}
	}

	void TCPSocket::Win32Obj::getLocalAddress(SocketAddress *pDest)
	{
		sockaddr_in localinfo;
		int size = sizeof(localinfo);
		if (getsockname(m_socket, (SOCKADDR*)&localinfo, &size)!=SOCKET_ERROR && size==sizeof(localinfo))
		{
			pDest->m_addr.full = localinfo.sin_addr.s_addr;
			pDest->m_port = IPAddress::networkToHostS(localinfo.sin_port);
		}
		else
		{
			czWarning(ID_Network, "Call to getsockname failed : %s", getErrorMessage());
		}
	}

	bool TCPSocket::Win32Obj::checkClosed(ClosedReason *reason)
	{
		return false;
	}


#endif	

} // namespace cz
