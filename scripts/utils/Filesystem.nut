/*******************************************************************************
* Miscenallenous functions to deal with files and directories
*******************************************************************************/

/**
Creates a directory, with any necessary parent directories
*/
function createPath(path)
{
	local dir = "";
	local parts = split(path, "\\/");
	foreach(p in parts) {
		dir += p + "\\";
		if (filetype(dir)==0) {
			if (!mkdir(dir))
				exitWithError(format("Couldn't create directory %s", dir));
		}
	}
}

// Copies a file, creating any required folders
function copyFile(src, dst, overwrite=false)
{
	local dstDir = filepath(dst);
	if (filetype(dstDir)!=DIR)
		createPath(dstDir)
		
	if (!copy(src,dst, overwrite))
		exitWithError(format("Couldn't copy file %s", filename(dst, true)));
}


/*
Copies an array of files from one folder to another, creating any required
directories
\param src
	Source folder
\param dst
	Destination folder
\param files
	Array of string, with the names of the files to copy
*/
function copyFiles(src, dst, files)
{
	foreach (f in files) {
		local from = src + f;
		local to = dst + f;
		printf("Copying %s\n", f)
		copyFile(from, to, true)
	}
}


