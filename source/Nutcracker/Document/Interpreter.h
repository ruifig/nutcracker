#pragma once

namespace cz
{
namespace document
{

class Variables
{
public:
	Variables(Variables* parent = nullptr);
	typedef std::function<UTF8String(void)> GenFunc;
	void set(UTF8String name, GenFunc genFunc);
	UTF8String replace(const UTF8String& str) const;
private:
	Variables* m_parent = nullptr;
	std::map<UTF8String, GenFunc> m_vars;
};

class Launcher
{

};

class Session
{
public:
private:
};

class Interpreter
{
public:
	Interpreter();
	virtual ~Interpreter();
	bool hasDebugger();
	std::unique_ptr<Session> launch(const Variables& variables, const UTF8String& file);
	const UTF8String& getName();

	static std::vector<std::unique_ptr<Interpreter>> initAll(const UTF8String& folder);
private:
	static std::unique_ptr<Interpreter> init(const UTF8String& cfgFile);
	UTF8String m_name;
	bool m_hasDebugger = false;
	UTF8String m_launch;
};

} // namespace document
} // namespace cz