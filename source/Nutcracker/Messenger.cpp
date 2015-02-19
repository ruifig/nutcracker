#include "NutcrackerPCH.h"
#include "Messenger.h"

namespace cz
{

Messenger::Messenger()
{
}

Messenger::~Messenger()
{
	if (m_isRunning)
	{
		m_shutdown = true;
		m_thread.join();
		m_sock.Shutdown();
	}
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
	m_thread = std::thread(&Messenger::run, this);
	czDebug(ID_Log, "Messenger: Connected");

	return true;
}

void Messenger::run()
{
	char part[1024];
	//std::queue<char> incoming;
	std::string incoming;

	while (!m_shutdown)
	{
		/*
		if (!m_sock.Wait(0, 10))
			continue;
			*/

		if (m_sock.IsDisconnected())
			break;

		// Read any available data
		m_sock.Read(part, sizeof(part));
		for (size_t i = 0; i < m_sock.LastReadCount(); i++)
		{
			if (part[i] == '\n')
			{
				czDebug(ID_Log, "Messenger: Receive: %s", incoming.c_str());
				m_in.push(std::move(incoming));
			}
			else
				incoming.push_back(part[i]);
		}

		std::string outgoing;
		if (m_out.try_and_pop(outgoing))
		{
			auto p = outgoing.begin();
			while (p != outgoing.end() && m_sock.IsConnected())
			{
				m_sock.Write(&(*p), outgoing.end() - p);
				p += m_sock.LastWriteCount();
			}
		}
	}

	czDebug(ID_Log, "Messenger: Disconnected");
}

bool Messenger::read(std::string& msg)
{
	return m_in.try_and_pop(msg);
}

void Messenger::send(std::string msg)
{
	czDebug(ID_Log, "Messenger: Send: %s", msg.c_str());
	m_out.push(std::move(msg));
}

}
