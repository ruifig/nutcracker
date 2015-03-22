/********************************************************************
	CrazyGaze (http://www.crazygaze.com)
	Author : Rui Figueira
	Email  : rui@crazygaze.com

	created:	2012/03/09
	created:	9:3:2012   0:39
	filename: 	czFileSystem.h
	
	purpose:
	
*********************************************************************/


#ifndef czFileSystem_h__
#define czFileSystem_h__

#include "czPlatformPrerequisites.h"
#include "czFilename.h"
#include "czSingleton.h"

namespace cz
{
	/*! Provides access to misc file/folder management functions.

	*/
	class Filesystem : public TAllocatedObject<GeneralCategoryAllocator>, public TSingleton<Filesystem>
	{
	public:
		Filesystem();
		virtual ~Filesystem();

		/*! Returns the current working directory.
		Current working directory is default directory used to look for/open/create files, when specifying
		relative paths.
		*/
		const Filename& getCWD() const;

		/*! Returns the original CWD when the process launched.
		*/
		const Filename& getOriginalCWD() const;

		/*! Changes the current working directory.
		This only works for platforms that allow writing/reading from several folders.
		Symbian for example only allows access to the app private folder (for normal apps).
		*/
		void setCWD(const UTF8String& path);

		/*! Changes the current working directory to a sensible platform default.
		On Windows, this will change the working directory to the process's location.
		On Symbian, it will change it to "/private/appUID/".
		*/
		void setCWDToDefault();

		static bool isExistingDirectory(const UTF8String& path);
		static bool isExistingFile(const UTF8String& filename);
		static bool createRelativePath(UTF8String& dst, const UTF8String& from, const UTF8String& to);
		static bool createDirectory(const UTF8String& path);


		enum class Type
		{
			Directory,
			File
		};
		struct FileInfo
		{
			Type type;
			Filename name;
			uint64_t size; // size, if it's a file
		};

		static std::vector<FileInfo> getFilesInDirectory(const UTF8String& path, const UTF8String& wildcard="*", bool includeDirectories=false);

		/*
		*/
		static bool isRelativePath(const UTF8String& path);

		/*
		Converts a path which has relative elements (e.g: . and .. ) to a well
		formed path.
		\param dst
			Where the computed path will be placed if the function succeeded
		\param path
			Path to convert
		\param root
			If the path is a relative path, this will prefixed
		*/
		static bool fullPath(UTF8String& dst, const UTF8String& path, const UTF8String& root=UTF8String(""));


		/*!
		Removes the path portion of a fully qualified path and file
		*/
		UTF8String pathStrip(const UTF8String& path);

	private:

		// Original "current working directory" when we launched the process.
		// I'm keeping this, so that 
		Filename mOriginalCWD;

		Filename mCWD;
	};
}

#endif // czFileSystem_h__
