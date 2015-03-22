/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com
	
	purpose:
*********************************************************************/

#pragma once

#include <memory>

namespace cz
{

	class SharedFlag
	{
	public:
		bool isValid() const;
	protected:
		friend class SharedFlagOwner; 
		std::shared_ptr<bool> m_flag;
	};

	class SharedFlagOwner
	{
	public:

		SharedFlagOwner();
		~SharedFlagOwner();
		/*!
		 * Copy constructor and assignment operator don't really copy anything from "other".
		 * They create their own flag. This is so that objects that have an instance of SharedFlagOwner
		 * can be copied around, creating their own independent SharedFlagOwner instance
		 */
		SharedFlagOwner(const SharedFlagOwner& other);
		SharedFlagOwner& operator=(const SharedFlagOwner& other);

		/**
		 * This should only be used internally
		 */
		SharedFlag _getFlag();

		/**
		 * Normally, the flag is set to false automatically in the destructor, but in 
		 * some situations, an explicit set my be desirable, instead of waiting for the object to go out
		 * of scope. This method allows for that explicit operatio.
		 */
		void explicitSet();
	private:
		SharedFlag m_flag;
	};

} // namespace cz

