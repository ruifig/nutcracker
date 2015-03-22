/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/02/21
	created:	21:2:2012   22:46
	filename: 	czFilename.h

	purpose:
	Deals with filenames, from which we can extract paths, base filename, extensions, etc
*********************************************************************/

#ifndef czFilename_h__
#define czFilename_h__

#include "czPlatformPrerequisites.h"

#include "czUTF8String.h"

namespace cz
{

	/*! \addtogroup IO
		@{
	*/


	/*! Manipulates filenames.
	You can use this class to manipulate filenames, and extract path, filename, extension, etc
	*/
	class Filename : public UTF8String
	{
	public:

		/*! Empty constructor */
		Filename() {}

		/*! Constructs a filename from the specified path */
		Filename(const UTF8String &path)
			: UTF8String(path)
		{}

		/*! Constructs copying from another filename */
		Filename(const Filename &other)
			: UTF8String(*reinterpret_cast<const UTF8String*>(&other))
		{}

		/*! */
		Filename(const char* path)
			: UTF8String(path)
		{}

		/*! Returns the full filename path (path+filename) */
		const UTF8String& getFullPath() const;

		/*! Returns the filename, including extension (no path) */
		UTF8String getFilename() const;

		/*! Returns the filename, without extension or path*/
		UTF8String getBaseFilename() const;

		/*! Returns the directory part of the filename */
		UTF8String getDirectory() const;

		/*! Returns the extension */
		UTF8String getExtension(bool includeDot=false) const;

		/*! Checks if the filename uses an absolute or relative path.
		Absolutes paths are paths are in the form:<br>
		C:\somedir\file.txt) <br>
		\somedir\file.txt <br><br>
		Relative paths are in the form:<br>
		somedir\file.txt
		.\somedir\file.txt
		..\somedir\file.txt
		<br><br>
		*/
		bool isRelativePath() const;


		UTF8String removeBackslash() const;

	private:

		void set(const UTF8String& path);
	};

	/*!
		@}
	*/
}

#endif // czFilename_h__
