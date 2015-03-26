#!/usr/bin/env squirrelsh

// Squirrel Shell archiving script
// Copyright (c) 2006-2012, Constantin Makshin
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

//-------------------------------------------------------------------------------------------------

// Some useful variables
local srcDirPath  = filepath(__argv[0]) + "/",
	  packageName = "squirrelsh-" + SHELL_VERSION,
	  tempDirPath = "/tmp/",
	  listPath	  = tempDirPath + packageName + "_" + filename(__argv[0]) + "_" + systime() + ".tmp",
	  destDirPath = tempDirPath + packageName + "/";

// Files that must be in all types of archives
local commonFiles = [
						"COPYING",
						"COPYING-pcre",
						"COPYING-squirrel",
						"COPYING-zlib",
						"HISTORY",
						"INSTALL",
						"README",
						"doc/squirrel3.pdf",
						"doc/squirrelsh.1",
						"doc/squirrelsh.pdf",
						"examples/hello.bat",
						"examples/hello.nut",
						"examples/hello.rb",
						"examples/hello.sh",
						"examples/hello2.nut",
						"examples/listdir.bat",
						"examples/listdir.nut",
						"examples/listdir.rb",
						"examples/listdir.sh",
						"examples/regexp.nut"
					];

// Files for binary archive
local binFiles = [
					 "bin/squirrelsh",
				 ];

// Files for source archive
local srcFiles = [
					 "archive.nut",
					 "configure",
					 "configure-mime.in",
					 "install-mime",
					 "make-debian-packages",
					 "Makefile.in",
					 "Makefile.mingw",
					 "Makefile.msvc",
					 "squirrelsh.sln"
					 "squirrelsh_vs71.sln",
					 "squirrelsh_vs8.sln",
					 "squirrelsh_vs9.sln",
					 "uninstall-mime",
					 "build/mingw.inc",
					 "build/msvc.inc",
					 "debian/changelog.in",
					 "debian/compat",
					 "debian/control",
					 "debian/copyright.in",
					 "debian/rules",
					 "doc/squirrelsh.tex",
					 "pcre/config.h",
					 "pcre/Makefile.in",
					 "pcre/Makefile.mingw",
					 "pcre/Makefile.msvc",
					 "pcre/pcre.h",
					 "pcre/pcre.vcxproj",
					 "pcre/pcre_vs71.vcproj",
					 "pcre/pcre_vs8.vcproj",
					 "pcre/pcre_vs9.vcproj",
					 "pcre/pcre_byte_order.c",
					 "pcre/pcre_chartables.c",
					 "pcre/pcre_compile.c",
					 "pcre/pcre_config.c",
					 "pcre/pcre_dfa_exec.c",
					 "pcre/pcre_exec.c",
					 "pcre/pcre_fullinfo.c",
					 "pcre/pcre_get.c",
					 "pcre/pcre_globals.c",
					 "pcre/pcre_jit_compile.c",
					 "pcre/pcre_internal.h",
					 "pcre/pcre_maketables.c",
					 "pcre/pcre_newline.c",
					 "pcre/pcre_ord2utf8.c",
					 "pcre/pcre_refcount.c",
					 "pcre/pcre_study.c",
					 "pcre/pcre_tables.c",
					 "pcre/pcre_ucd.c",
					 "pcre/pcre_valid_utf8.c",
					 "pcre/pcre_version.c",
					 "pcre/pcre_xclass.c",
					 "pcre/ucp.h",
					 "shell/Makefile.in",
					 "shell/Makefile.mingw",
					 "shell/Makefile.msvc",
					 "shell/base.cpp",
					 "shell/common.h",
					 "shell/file.cpp",
					 "shell/hash.cpp",
					 "shell/hash.h",
					 "shell/hash_adler32.cpp",
					 "shell/hash_crc32.cpp",
					 "shell/hash_md5.cpp",
					 "shell/io.cpp",
					 "shell/main.cpp",
					 "shell/math.cpp",
					 "shell/regexp.cpp",
					 "shell/platform.h",
					 "shell/resource.h",
					 "shell/shell.rc",
					 "shell/shell.vcxproj",
					 "shell/shell_vs71.vcproj",
					 "shell/shell_vs8.vcproj",
					 "shell/shell_vs9.vcproj",
					 "shell/squirrelsh.ico",
					 "shell/util.cpp",
					 "shell/version.h",
					 "squirrel/Makefile.in",
					 "squirrel/Makefile.mingw",
					 "squirrel/Makefile.msvc",
					 "squirrel/sqapi.cpp",
					 "squirrel/sqarray.h",
					 "squirrel/sqbaselib.cpp",
					 "squirrel/sqclass.cpp",
					 "squirrel/sqclass.h",
					 "squirrel/sqclosure.h",
					 "squirrel/sqcompiler.cpp",
					 "squirrel/sqcompiler.h",
					 "squirrel/sqdebug.cpp",
					 "squirrel/sqfuncproto.h",
					 "squirrel/sqfuncstate.cpp",
					 "squirrel/sqfuncstate.h",
					 "squirrel/sqlexer.cpp",
					 "squirrel/sqlexer.h",
					 "squirrel/sqmem.cpp",
					 "squirrel/sqobject.cpp",
					 "squirrel/sqobject.h",
					 "squirrel/sqopcodes.h",
					 "squirrel/sqpcheader.h",
					 "squirrel/sqstate.cpp",
					 "squirrel/sqstate.h",
					 "squirrel/sqstring.h",
					 "squirrel/sqtable.cpp",
					 "squirrel/sqtable.h",
					 "squirrel/squirrel.h",
					 "squirrel/squirrel.vcxproj",
					 "squirrel/squirrel_vs71.vcproj",
					 "squirrel/squirrel_vs8.vcproj",
					 "squirrel/squirrel_vs9.vcproj",
					 "squirrel/squserdata.h",
					 "squirrel/squtils.h",
					 "squirrel/sqvm.cpp",
					 "squirrel/sqvm.h",
				 ];

// Command line arguments passed to 'tar'
local tarArgs = [
					"--create",
					"--owner=root",
					"--group=root",
					"--directory=" + tempDirPath,
					"--files-from=" + listPath
				];

// Archiving options
local archiveType	= "src",   // Type of archive ("bin", "src", "full")
	  archiveFormat = "bz2",   // Format of archive ("gz", "bz2", "xz")
	  compressor	= "bzip2"; // Compression program ("gzip", "bzip2", "xz")

//-------------------------------------------------------------------------------------------------

function AddFiles (file, list)
{
	foreach (entry in list)
	{
		if (exist(srcDirPath + entry))
		{
			local destDir = filepath(destDirPath + entry);
			if (!exist(destDir) && !mkdir(destDir))
			{
				printl("Couldn't create directory '" + destDir + "'.");
				continue;
			}

			copy(srcDirPath + entry, destDirPath + entry, true);
			fprint(file, packageName + "/" + entry + "\n");
		}
		else
			printl("File '" + entry + "' not found.");
	}
}

function Close (exitCode = 0)
{
	remove(listPath);
	rmdir(destDirPath, true);
	exit(exitCode);
}

//-------------------------------------------------------------------------------------------------

// Parse command line arcguments
for (local i = 1; i < __argc; ++i)
{
	local arg = __argv[i];
	if ((arg == "-h") || (arg == "--help"))
	{
		// Display usage information and quit
		printl("Squirrel Shell archiving script");
		printl("Copyright (c) 2006-2012, Constantin Makshin\n");
		printl("Usage:");
		printl("    " + filename(__argv[0], true) + " [options]\n");
		printl("Options:");
		printl("    -h,   --help      Display this text");
		printl("    -b,   --binary    Create archive with only shell executable ('bin/squirrelsh')");
		printl("    -s,   --source    Create archive with only files required to compile shell (default)");
		printl("    -f,   --full      Create archive with both shell executable and sources");
		printl("    -gz,  --gzip      Create archive in .tar.gz format");
		printl("    -bz2, --bzip2     Create archive in .tar.bz2 format (default)");
		printl("    -xz,  --xz        Create archive in .tar.xz format\n");
		printl("If more than one option of the same purpose is specified, the last one will be accepted. For example, running 'archive.nut -b -f -gz -bz2' will result in full archive compressed with bzip2.");
		return;
	}
	else if ((arg == "-b") || (arg == "--binary"))
		archiveType = "bin";
	else if ((arg == "-s") || (arg == "--source"))
		archiveType = "src";
	else if ((arg == "-f") || (arg == "--full"))
		archiveType = "full";
	else if ((arg == "-gz") || (arg == "--gzip"))
	{
		archiveFormat = "gz";
		compressor	  = "gzip";
	}
	else if ((arg == "-bz2") || (arg == "--bzip2"))
	{
		archiveFormat = "bz2";
		compressor	  = "bzip2";
	}
	else if ((arg == "-xz") || (arg == "--xz"))
	{
		archiveFormat = "xz";
		compressor	  = "xz";
	}
	else
	{
		printl("Invalid option: " + arg);
		return;
	}
}

// Write files list
local f = fopen(listPath, WRITE_ONLY);
if (!f)
{
	printl("Failed to create/open file '" + listPath + "'.");
	Close(1);
}

AddFiles(f, commonFiles);

if ((archiveType == "bin") || (archiveType == "full"))
	AddFiles(f, binFiles);
if ((archiveType == "src") || (archiveType == "full"))
	AddFiles(f, srcFiles);

fclose(f);
printl("Built list of files for archiving.");

// Build full name of archive and add it to tar's options
local archiveName = getcwd() + "/" + packageName + "-" + archiveType + ".tar";
tarArgs.append("--file=" + archiveName);

// Create archive
if (run("tar", tarArgs))
	Close(1);

printl("Built archive '" + archiveName + "'.");
local tmpName = archiveName + "." + archiveFormat;
if (exist(tmpName))
	remove(tmpName);
if (run(compressor, [ archiveFormat == "xz" ? "-6" : "-9", archiveName ]))
	Close(1);

archiveName += "." + archiveFormat;
printl("Compressed archive to '" + archiveName + "'.");

local md5Name = archiveName + ".md5";
f = fopen(md5Name, WRITE_ONLY);
if (f)
{
	fprint(f, md5(archiveName) + " *" + archiveName + "\n");
	fclose(f);
	printl("Wrote MD5 sum to '" + md5Name + "'.");
}

Close();
