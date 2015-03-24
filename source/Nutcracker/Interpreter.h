#pragma once
#include "UIDefs.h"

namespace nutcracker
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

template<class T>
struct Listeners
{
public:
	void add(void* listener, T&& func)
	{
		m_all[listener] = std::forward<T>(func);
	}
	void remove(void* listener)
	{
		m_all.erase(listener);
	}

	void fire()
	{
		for (auto& i : m_all)
			i.second();
	}

	template<class A1>
	void fire(A1&& a1)
	{
		for (auto& i : m_all)
			i.second(std::forward<A1>(a1));
	}

	template<class A1, class A2>
	void fire(A1&& a1, A2&& a2)
	{
		for (auto& i : m_all)
			i.second(std::forward<A1>(a1), std::forward<A2>(a2));
	}

private:
	std::unordered_map<void*, T> m_all;
};

enum class BreakType
{
	Step,
	Breakpoint,
	Error
};

enum class VarType
{
	Null,
	Root,
	String,
	Integer,
	Float,
	UserData,
	Function,
	Table,
	Array,
	Generator,
	Thread,
	Instance,
	Class,
	Bool,
	WeakRef,
	MAX,
};

struct ValueBase
{
	ValueBase(VarType type) : type(type) { }
	virtual ~ValueBase() {}
	VarType type;
	std::string txt;
	virtual bool isTable() const {
		return false;
	}
};

struct ValueNull : public ValueBase
{
	ValueNull() : ValueBase(VarType::Null)
	{
		txt = "null";
	}
};

struct ValueString : public ValueBase
{
	ValueString(const char* str) : ValueBase(VarType::String)
	{
		txt = str;
	}
};

struct ValueInteger : public ValueBase
{
	ValueInteger(int val) : ValueBase(VarType::Integer)
	{
		txt = std::to_string(val);
	}
};

struct ValueFloat : public ValueBase
{
	ValueFloat(float val) : ValueBase(VarType::Float)
	{
		txt = std::to_string(val);
	}
};

struct ValueUserData : public ValueBase
{
	ValueUserData() : ValueBase(VarType::UserData)
	{
		txt = "userdata";
	}
};

struct ValueFunction : public ValueBase
{
	ValueFunction() : ValueBase(VarType::Function)
	{
		txt = "function";
	}
};

struct TableEntry
{
	std::shared_ptr<ValueBase> key;
	std::shared_ptr<ValueBase> val;
	struct ValueBaseTable* outer=nullptr;

	std::string getName() const;
	std::string getValue() const;
	std::string getType() const;
};

struct ValueBaseTable : public ValueBase
{
	ValueBaseTable(VarType type) : ValueBase(type)
	{
	}

	virtual bool isTable() const override {
		return true;
	}

	std::vector<TableEntry> entries;
};

struct ValueRoot : public ValueBaseTable
{
	ValueRoot() : ValueBaseTable(VarType::Root)
	{
		txt = "root";
	}
};

struct ValueTable : public ValueBaseTable
{
	ValueTable() : ValueBaseTable(VarType::Table)
	{
		txt = "table";
	}
};

struct ValueArray : public ValueBaseTable
{
	ValueArray() : ValueBaseTable(VarType::Array)
	{
		txt = "array";
	}
};

struct ValueGenerator : public ValueBase
{
	ValueGenerator() : ValueBase(VarType::Generator)
	{
		txt = "generator";
	}
};

struct ValueThread : public ValueBase
{
	ValueThread() : ValueBase(VarType::Thread)
	{
		txt = "thread";
	}
};

struct ValueInstance : public ValueBaseTable
{
	ValueInstance() : ValueBaseTable(VarType::Instance)
	{
		txt = "Instance";
	}
};

struct ValueClass : public ValueBaseTable
{
	ValueClass() : ValueBaseTable(VarType::Class)
	{
		txt = "class";
	}
};

struct ValueBool : public ValueBase
{
	ValueBool(bool val) : ValueBase(VarType::Bool)
	{
		txt = std::to_string(val);
	}
};

struct ValueWeakRef : public ValueBase
{
	ValueWeakRef() : ValueBase(VarType::WeakRef)
	{
		txt = "weakref";
	}
};

struct Variable
{
	std::string name;
	std::shared_ptr<ValueBase> val;
};

enum class WatchState
{
	Unknown,
	Valid,
	Invalid
};

struct WatchValue : public TableEntry
{
	int id = 0;
	WatchState state = WatchState::Unknown;
};

struct CallstackEntry
{
	std::string func;
	std::shared_ptr<const File> file;
	UTF8String filename;
	int line;
	std::vector<TableEntry> locals;
	std::map<int, WatchValue> watches;
};

struct BreakInfo
{
	BreakType type=BreakType::Error;
	std::shared_ptr<const File> file;
	int line=-1;
	std::string error;

	std::map<int, std::shared_ptr<ValueBase>> objs;

	int currentCallstackFrame = 0;
	std::vector<CallstackEntry> callstack;
	int customObjKeyCounter = -1;
};

class DebugSession : public std::enable_shared_from_this<DebugSession>
{
public:
	DebugSession();
	~DebugSession();
	
	// If port is 0, it will not use a debugger;
	bool start(const std::string& ip, int port);

	Listeners < std::function<void()>> resumeListeners;
	Listeners < std::function<void(int, std::shared_ptr<const File>)>> addBreakpointListeners;
	Listeners < std::function<void(const std::shared_ptr<BreakInfo>&)>> breakListeners;
	Listeners < std::function<void()> > disconnectListeners;

	void removeListener(void* listener)
	{
		resumeListeners.remove(listener);
		addBreakpointListeners.remove(listener);
		breakListeners.remove(listener);
		disconnectListeners.remove(listener);
	}

	void stepInto();
	void stepReturn();
	void stepOver();
	void resume();
	void suspend();
	void terminate();

	void removeBreakpoint(Breakpoint* brk);
	void updateBreakpoint(Breakpoint* brk);

	void removeWatch(int id);
	void addWatch(int id, const std::string& exp);

private:
	void processIncoming();
	void processMsgResumed(tinyxml2::XMLElement* xml);
	void processMsgAddBreakpoint(tinyxml2::XMLElement* xml);
	void processMsgBreak(tinyxml2::XMLElement* xml);
	void processObjElement(tinyxml2::XMLElement* ele, BreakInfo& info);
	void processCallElement(tinyxml2::XMLElement* ele, BreakInfo& info);
	std::shared_ptr<ValueBase> processValue(tinyxml2::XMLElement* ele, BreakInfo& info, const char* attrType, const char* attrVal=nullptr);
	std::unique_ptr<class Messenger> m_messenger;
	bool m_paused=false;
};

class Interpreter
{
public:
	Interpreter();
	virtual ~Interpreter();
	bool hasDebugger() const;
	bool launch(const Variables& variables, const UTF8String& file);
	std::shared_ptr<DebugSession> launchDebug(const Variables& variables, const UTF8String& file);
	const UTF8String& getName() const;

	static std::vector<std::unique_ptr<Interpreter>> initAll(const UTF8String& folder);
private:
	static std::unique_ptr<Interpreter> init(const UTF8String& cfgFile);
	UTF8String m_name;
	bool m_hasDebugger = false;
	UTF8String m_launchNormal;
	UTF8String m_launchDebug;
	std::string m_ip;
	int m_port;
};

} // namespace nutcracker