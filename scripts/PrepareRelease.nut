dofile("utils.nut")

function originPath(file)
{
	local res = filepath(__argv[0]) + "\\..\\" + file;
	return res;
}

function distPath(path)
{
	return originPath("tmp\\distribution\\") + path;
}

function testMSBuild()
{
	if (run("msbuild","/version", null, true, true)==-1)
		exitWithError("msbuild not found")
}

function copyToDist(src, dst)
{
	src = originPath(src)
	dst = distPath(dst);
	printf("Copying %s to %s\n", src, dst);
	local res = copy(src, dst, false);
}


print("\n")
testMSBuild()

//
// Build
build("dependencies\\squirrel\\squirrel.sln");
//build("dependencies\\squirrelshell\\squirrelsh.sln");
//build("source\\Nutcracker.sln");
exit(1)
//
// Prepare distribution folder
//
rmdir(distPath(""), true)
mkdir(distPath(""))
mkdir(distPath("bin"))
mkdir(distPath("bin\\interpreters"))
mkdir(distPath("dependencies"))
mkdir(distPath("dependencies\\squirrel"))
mkdir(distPath("dependencies\\squirrel\\bin"))
mkdir(distPath("dependencies\\squirrelshell"))
mkdir(distPath("dependencies\\squirrelshell\\bin"))

//
// Copy files
//
copyToDist("bin\\Nutcracker_Win32_Release.exe", "bin\\Nutcracker.exe")
copyToDist("bin\\config.ini", "bin\\config.ini")
copyToDist("bin\\interpreters\\sq.ini", "bin\\interpreters\\sq.ini")
copyToDist("bin\\interpreters\\squirrelshell.ini", "bin\\interpreters\\squirrelshell.ini")
/*
copyToDist("dependencies\\squirrel\\bin\\sq.exe", "dependencies\\squirrel\\bin\\sq.exe")
copyToDist("dependencies\\squirrel\\bin\\sqdbg.exe", "dependencies\\squirrel\\bin\\sqdbg.exe")
copyToDist("dependencies\\squirrelshell\\bin\\squirrelsh.exe", "dependencies\\squirrelshell\\bin\\squirrelsh.exe")
*/

copyToDist("dependencies\\squirrel\\bin\\*.exe", "dependencies\\squirrel\\bin");

print("\nOK\n")

