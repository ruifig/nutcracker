/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose: Handles TCP connections
	
*********************************************************************/

#ifndef _czTCPSocket_h_
#define _czTCPSocket_h_


#include "czPlatformPrerequisites.h"
#include "czString.h"
#include "czRingBuffer.h"
#include "czArray.h"
#include "czSingleton.h"

namespace cz
{


	class clcpp_attr(reflect,serialize) IPAddress
	{
	public:

		union
		{
			#ifndef __clcpp_parse__
			struct bytes_st{
				u8 b1, b2, b3, b4;
			} bytes;
			#endif
			u32 full; 
		};

	public:
		IPAddress()
		{
			full = 0;
		}
		#ifndef __clcpp_parse__
		IPAddress(u8 b1, u8 b2, u8 b3, u8 b4)
		{
			bytes.b1 = b1;
			bytes.b2 = b2;
			bytes.b3 = b3;
			bytes.b4 = b4;
		}
		#endif

		IPAddress(u32 full_)
		{
			full = full_;
		}

		IPAddress(const char* str);

		bool operator==(const IPAddress& other) const
		{
			return full==other.full;
		}

		bool operator<(const IPAddress& other) const
		{
			return full<other.full;
		}

		const char* toString() const;

		static bool createFrom(const char* str, IPAddress &dest);
		static u32 hostToNetworkL(u32 host);
		static u16 hostToNetworkS(u16 host);
		static u32 networkToHostL(u32 network);
		static u16 networkToHostS(u16 network);

		friend class SocketAddress;
		friend class TCPSocket;
	};




	class clcpp_attr(reflect,serialize) SocketAddress
	{
	public:
		SocketAddress()
		{
			m_port = 0;
		}
		SocketAddress(const IPAddress &ip, u16 port) : m_addr(ip), m_port(port)
		{
		}
		SocketAddress(u32 ip, u16 port) : m_addr(ip), m_port(port)
		{
		}
		SocketAddress(const char *str)
		{
			m_port = 0;
			createFrom(str, *this);
		}

		bool operator==(const SocketAddress& other) const
		{
			return (m_addr==other.m_addr) && (m_port==other.m_port);
		}

		bool operator< (const SocketAddress& other) const
		{
			return (m_addr<other.m_addr) && (m_port<other.m_port);
		}

		const char* toString() const;

		static bool createFrom(const char *str, SocketAddress &dest);

		friend class CTCPSocket;

	public:
		IPAddress m_addr;
		u16 m_port;
	};

	String& operator<< (String& s, const IPAddress& val);
	String& operator<< (String& s, const SocketAddress& val);


	class TCPSocket : public TAllocatedObject<GeneralCategoryAllocator>
	{
	protected:
		friend class TCPSocketManager;
		TCPSocket();
		~TCPSocket();
		void tick();
		bool initialize();

		static const char* getErrorMessage(int err=0);
	public:
		enum State
		{
			kSOCKET_DISCONNECTED,
			kSOCKET_LISTENING,
			kSOCKET_CONNECTING,
			kSOCKET_CONNECTED,
			kSOCKET_DISCONNECTING
		};

		enum ClosedReason
		{
			kSOCKET_CLOSED_REASON_NORMAL, //! A normal close done by the peer socket
			kSOCKET_CLOSED_REASON_SYSTEM //! An internal error occurred
		};

		const SocketAddress& getLocalAddress() const
		{
			return m_localAddr;
		}
		const SocketAddress& getPeerAddress() const
		{
			return m_peerAddr;
		}

		void setCookie(void* cookie)
		{
			m_cookie = cookie;
		}

		void* getCookie() const
		{
			return m_cookie;
		}

		/*!
		 * \brief Sets the listener.
		 * Through the listener you get callbacks for useful stuff
		 */
		void setListener(class TCPSocketListener *pListener);

		//! Closes the socket
		bool close();

		/**
		 * \brief	Binds the socket with a local address
		 *
		 * This is only to be used when using the socket as a server, to specify IP and port to listen
		 * \param	addr	The address. 
		 **/
		bool bind(const SocketAddress &addr);

		/**
		 * \brief	Starts listening for incoming connections
		 * 			
		 *  This is only to be used when using the socket as a server, to start waiting for incoming connections
		 * 
		 *  \param backlog Maximum length of the pending connections queue. -1 will use default value
		 *	provided by the OS
		 *
		 **/
		bool listen(int  backlog=-1);

		/**
		 * \brief Connects to the specified address
		 */
		bool connect(const SocketAddress &addr);

		/**
		 * \brief Sends data to the peer
		 *
		 */
		bool send(const void *buf, int size);

		State getState()
		{
			return m_state;
		}

		RingBuffer* getReceiveBuffer()
		{
			return &m_recvbuffer;
		}
		RingBuffer* getSendBuffer()
		{
			return &m_sendbuffer;
		}
		uint32_t getBufferedSendSize() const
		{
			return m_sendbuffer.getUsedSize();
		}

	private:

		bool doSend();
		bool doReceive();
		void setStatusFlags(bool bReadable, bool bWritable, bool bError);

		SocketAddress m_localAddr;
		SocketAddress m_peerAddr;
		class TCPSocketListener *m_pListener;
		void* m_cookie; // Any data the class user wants to put in here
		State m_state;
		RingBuffer m_sendbuffer;
		RingBuffer m_recvbuffer;
		bool m_bReadyToSend;
		bool m_bReadyToRead;
		bool m_bError;
		bool m_pendingDestroy;

		enum OperationResult
		{
			kRESULT_OK,
			kRESULT_PEER_SHUTDOWN,
			kRESULT_ERROR
		};

#if CZ_PLATFORM==CZ_PLATFORM_WIN32
		class Win32Obj
		{

		private:
			SOCKET m_socket;
		public:
			TCPSocket *outer;
			Win32Obj();
			~Win32Obj();
			SOCKET& getOSSocket()
			{
				return m_socket;
			}

			static const char* getErrorMessage(int err=0);
			TCPSocket::OperationResult initialize();
			void getLocalAddress(SocketAddress *pDest);
			TCPSocket::OperationResult close();
			TCPSocket::OperationResult bind(const SocketAddress &addr);
			TCPSocket::OperationResult listen(int  backlog);
			TCPSocket::OperationResult accept(TCPSocket** ppNewPeer);
			TCPSocket::OperationResult connect(const SocketAddress &addr);
			TCPSocket::OperationResult send(void *buf, int size, int *sentsize);
			TCPSocket::OperationResult receive(void *buf, int maxsize, int *receivedsize);
			bool checkClosed(ClosedReason *reason);
		} m_osdata;
#else
	#error Platform not supported
#endif

	};



	class TCPSocketListener
	{
	public:

		/*!
		 * \brief Called when a socket that is listening received a new incoming connection
		 * 	
		 * \param socket Socket this was generated from (the listening socket)
		 * \param pNewPeer
		 *		Socket with the new connection.
		 */
		virtual void onSocketAccept(TCPSocket &socket, TCPSocket *pNewPeer) = 0;

		/*!
		 * \brief Called when the socket is closed
		 */
		virtual void onSocketClosed(TCPSocket &socket, TCPSocket::ClosedReason reason) = 0 ;
		/*!
		 * \brief Called when a socket successfully connects to a peer
		 */
		virtual void onSocketConnected(TCPSocket &socket) = 0;

		/*!
		 * \brief Called if a connection attempt failed
		 */
		virtual void onSocketConnectFailed(TCPSocket &socket) = 0;

		/*!
		 * \brief Called when data is received
		 Use getReceiveBuffer() to read incoming data.
		 */
		virtual void onSocketReceived(TCPSocket &socket) = 0;

	};

	class TCPSocketManager : public TSingleton<TCPSocketManager>, public TAllocatedObject<GeneralCategoryAllocator>
	{
	public:
		TCPSocketManager();
		~TCPSocketManager();
		bool initialize();
		float tick(float deltaseconds) override;
		TCPSocket* createSocket();
		void destroySocket(TCPSocket* pSocket);
	private:
		void destroyPending();
		friend class TCPSocket;
		TArray<TCPSocket*> m_sockets;
	};

} // namespace cz

#endif // _czTCPSocket_h_
