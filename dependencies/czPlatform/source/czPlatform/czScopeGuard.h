/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:

	Based on ScopedGuard presented at
	http://channel9.msdn.com/Shows/Going+Deep/C-and-Beyond-2012-Andrei-Alexandrescu-Systematic-Error-Handling-in-C
*********************************************************************/

#pragma once
#ifndef _czScopeGuard_h_
#define _czScopeGuard_h_

namespace cz
{

template<class Func>
class ScopeGuard
{
public:
	ScopeGuard(Func f)
		: m_fun(std::move(f))
		, m_active(true)
	{
	}

	~ScopeGuard()
	{
		if (m_active)
			m_fun();
	}

	void dismiss()
	{
		m_active = false;
	}

	ScopeGuard() = delete;
	ScopeGuard(const ScopeGuard&) = delete;
	ScopeGuard& operator=(const ScopeGuard&) = delete;
	ScopeGuard(ScopeGuard&& rhs)
		: m_func(std::move(rhs.m_func))
		, m_active(rhs.active)
	{
		rhs.dismiss();
	}

private:
	Func m_fun;
	bool m_active;
};


/**
	Using a template function to create guards, since template functions can do type deduction,
	meaning shorter code.

	auto g1 = scopeGuard( [&] { cleanup(); } );
*/
template< class Func>
ScopeGuard<Func> scopeGuard(Func f)
{
	return ScopeGuard<Func>(std::move(f));
}

} // namespace cz


/**
	Some macro magic so it's easy to set anonymous scope guards. e.g:

	// some code ...
	SCOPE_EXIT { some cleanup code };
	// more code ...
	SCOPE_EXIT { more cleanup code };
	// more code ...
 */
namespace detail
{
	enum class ScopeGuardOnExit {};
	template <typename Func>
	__forceinline cz::ScopeGuard<Func> operator+(ScopeGuardOnExit, Func&& fn) {
		return cz::ScopeGuard<Func>(std::forward<Func>(fn));
	}
}

#define CONCATENATE_IMPL(s1,s2) s1##s2
#define CONCATENATE(s1,s2) CONCATENATE_IMPL(s1,s2)

// Note: __COUNTER__ Expands to an integer starting with 0 and incrementing by 1 every time it is used in a source file or included headers of the source file.
#ifdef __COUNTER__
	#define ANONYMOUS_VARIABLE(str) \
		CONCATENATE(str,__COUNTER__)
#else
	#define ANONYMOUS_VARIABLE(str) \
		CONCATENATE(str,__LINE__)
#endif

#define SCOPE_EXIT \
	auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) \
	= ::detail::ScopeGuardOnExit() + [&]()


#endif // _czScopeGuard_h_