#pragma once

namespace nutcracker
{

class Messenger
{
public:
	Messenger();
	~Messenger();
	bool connect(const char* ip, unsigned int port, unsigned timeoutSeconds);
	void send(std::string msg);
	bool read(std::string& msg);
	void setOnIncoming(std::function<void()> callback);
	void setOnDisconnected(std::function<void()> callback);

private:

	cz::SharedQueue<std::string> m_in;
	// For outgoing, I'm using a queue of commands, instead of strings, so we can have the writer thread
	// block waiting for work, and we can cancel that wait by sending a command to break the loop
	cz::SharedQueue<std::function<void()>> m_out;

	std::thread m_readThread;
	std::thread m_writeThread;
	wxSocketClient m_sock;
	volatile bool m_shutdown = false;
	volatile bool m_isRunning = false;
	std::function<void()> m_onIncomingCallback;
	std::function<void()> m_onDisconnectedCallback;
	void run_read();
	void run_write();
};

} // namespace nutcracker
