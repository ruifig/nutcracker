#!/usr/bin/env squirrelsh

// List directory contents.

local entries = readdir((__argc >= 2) ? __argv[1] : ".");
if (entries)
{
	foreach (entry in entries)
	{
		switch (filetype(entry))
		{
		case DIR:
			printl("dir : " + entry);
			break;

		case FILE:
			printl("file: " + entry);
			break;

		default:
		}
	}
}
