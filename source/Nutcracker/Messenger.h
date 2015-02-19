#pragma once

namespace cz
{

class Messenger
{
public:
	Messenger();
	~Messenger();
	bool connect(const char* ip, unsigned int port, unsigned timeoutMs);
	void send(std::string msg);
	bool read(std::string& msg);
private:
	cz::SharedQueue<std::string> m_out;
	cz::SharedQueue<std::string> m_in;
	std::thread m_thread;
	wxSocketClient m_sock;
	volatile bool m_shutdown = false;
	volatile bool m_isRunning = false;
	void run();
};

}
