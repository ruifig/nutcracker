/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Based on Herb Sutter monitor<T> presented at:
	http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Herb-Sutter-Concurrency-and-Parallelism
*********************************************************************/

#pragma  once

#include <mutex>

namespace cz
{

template <class T>
class Monitor
{
private:
	mutable T m_t;
	mutable std::mutex m_mtx;
public:
	Monitor(T t_ = T{})
		: m_t{t_}
	{
	}

	template< typename F >
	auto operator() (F f) const -> decltype( f(m_t))
	{
		std::lock_guard<std::mutex> l{m_mtx};
		return f(m_t);
	}
};

} // namespace cz
