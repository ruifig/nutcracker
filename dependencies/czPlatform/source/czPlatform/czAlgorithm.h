/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
	Some functions to simplify some STL methods
*********************************************************************/

#pragma once
#ifndef _czAlgorithm_h_
#define _czAlgorithm_h_

#include <functional>

namespace cz
{
	/*
	Same as std::find, but uses the full container range.
	*/
	template<typename C, typename T>
	auto find(C& c, const T& v) -> decltype(c.begin())
	{
		return std::find(c.begin(), c.end(), v);
	}
	
	/*
	Same as std::find_if, but uses the full container range.
	*/
	template<typename C, typename F>
	auto find_if(C& c, const F& f) -> decltype(c.begin())
	{
		return std::find_if(c.begin(), c.end(), f);
	}


	template<typename C, typename T>
	bool exists(C& c, const T& v)
	{
		return cz::find(c, v)!=c.end();
	}
	template<typename C, typename F>
	auto exists_if(C& c, const F& f) -> decltype(c.begin())
	{
		return std::find_if(c.begin(), c.end(), f)!=c.end();
	}



	/*
	Less verbose way to remove items from a container
	*/
	template<typename C, typename T>
	auto erase(C& c, const T& v) -> decltype(c.begin())
	{
		return c.erase(
			std::remove(c.begin(), c.end(), v),
			c.end());
	}

	/*
	Less verbose way to remove items from a container with a predicate
	*/
	template<typename C, typename F>
	auto erase_if(C& c, const F& f) -> decltype(c.begin())
	{
		return c.erase(
			std::remove_if(c.begin(), c.end(), f),
			c.end());
	}

	/*
	* For use with maps only
	*/
	template< typename Container, typename Predicate>
	void map_erase_if(Container& items, const Predicate& predicate)
	{
		for (auto it = items.begin(); it != items.end();) {
			if (predicate(*it))
				it = items.erase(it);
			else
				++it;
		}
	}

	/*
	Returns a new container with items that fulfill the predicate
	*/
	template<typename C, typename F>
	C copyfrom_if(const C& c, const F& f)
	{
		C res;
		for(const auto& i : c)
			if (f(i))
				res.push_back(i);
		return res;
	}

} // namespace cz

#endif // _czAlgorithm_h_
