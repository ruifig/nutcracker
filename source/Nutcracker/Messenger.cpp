#include "NutcrackerPCH.h"
#include "Messenger.h"

namespace nutcracker
{

Messenger::Messenger()
{
}

Messenger::~Messenger()
{
	if (m_isRunning)
	{
		m_shutdown = true;
		m_sock.Shutdown();
		m_out.push([](){}); // Push an empty command, just to wake up the thread and detect that m_shutdown is true
		m_readThread.join();
		m_writeThread.join();
	}
}

void Messenger::setOnIncoming(std::function<void()> callback)
{
	m_onIncomingCallback = std::move(callback);
}

void Messenger::setOnDisconnected(std::function<void()> callback)
{
	m_onDisconnectedCallback = std::move(callback);
}

bool Messenger::connect(const char* ip, unsigned int port, unsigned timeoutMs)
{
	wxIPV4address addr;
	addr.Hostname(ip);
	addr.Service(port);
	czDebug(ID_Log, "Messenger: Trying to connect to %s:%u", ip, port);
	m_sock.SetTimeout(timeoutMs);
	//m_sock.SetFlags(wxSOCKET_WAITALL);
	m_sock.SetFlags(wxSOCKET_NOWAIT);
	m_sock.Notify(false);
	m_sock.Connect(addr, true);

	if (!m_sock.WaitOnConnect())
	{
		czDebug(ID_Log, "Messenger: Failed to connect");
		return false;
	}

	if (!m_sock.IsConnected())
	{
		czDebug(ID_Log, "Messenger: Server refused connection request");
		return false;
	}

	m_isRunning = true;
	m_readThread = std::thread(&Messenger::run_read, this);
	m_writeThread = std::thread(&Messenger::run_write, this);
	czDebug(ID_Log, "Messenger: Connected");

	return true;
}

void Messenger::run_read()
{
	char part[4096];
	std::string incoming;
	while (!m_shutdown)
	{
		if (!m_sock.WaitForRead(1))
			continue;
		if (m_sock.IsDisconnected())
		{
			// If this was not an explicit shutdown, we need to let the write thread know we have a disconnect.
			// This is necessary because the writer thread blocks waiting for commands, instead of blocking on the socket,
			// so it can't detect a disconnect when blocking.
			if (!m_shutdown)
			{
				m_shutdown = true;
				czDebug(ID_Log, "Messenger: Disconnect detected.");
				m_out.push([](){});
				if (m_onDisconnectedCallback)
					m_onDisconnectedCallback();
			}

			break;
		}

		m_sock.Read(part, sizeof(part));
		auto count = m_sock.LastReadCount();
		for (size_t i = 0; i<count; i++)
		{
			if (part[i] == '\n')
			{
				czDebug(ID_Log, "Messenger: Receive: %s", incoming.c_str());
				m_in.push(std::move(incoming));
				if (m_onIncomingCallback)
					m_onIncomingCallback();
			}
			else
			{
				incoming.push_back(part[i]);
			}
		}
	}

	czDebug(ID_Log, "Messenger: Read thread finishing");
}

void Messenger::run_write()
{
	while (!m_shutdown)
	{
		std::function<void()> f;
		m_out.wait_and_pop(f);
		f();
	}
	czDebug(ID_Log, "Messenger: Write thread finishing");
}

bool Messenger::read(std::string& msg)
{
	return m_in.try_and_pop(msg);
}

void Messenger::send(std::string msg)
{
	czDebug(ID_Log, "Messenger: Send: %s", msg.c_str());
	m_out.push([msg,this]()
	{
		auto p = msg.begin();
		while (p != msg.end() && m_sock.IsConnected())
		{
			m_sock.WaitForWrite();
			if (m_sock.IsDisconnected())
				break;

			m_sock.Write(&(*p), msg.end() - p);
			p += m_sock.LastWriteCount();
		}
	});
}

} // namespace nutcracker

