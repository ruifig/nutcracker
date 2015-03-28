dofile("utils/Common.nut")
dofile("utils/Filesystem.nut")

/**!
Tests if MSBuild.exe is reachable
*/
function testMSBuild()
{
	if (run("msbuild","/version", null, true, true)==-1)
		exitWithError("msbuild not found")
}

/**!
Builds the specified solution
*/
function buildSolution(solution, target, platform, configuration)
{
	testMSBuild()

	local params = [
		solution,
		"/t:"+target,
		"/p:Platform="+platform,
		"/p:Configuration="+configuration,
		"/maxcpucount",
		"/nr:false" // Disable node reuse (so no instance of MSBuild stay open)
	]
	
	printf("Building %s...\n", filename(solution))
	if (run("msbuild", params, null, false, false)!=0)
		exitWithError("Failed to build " + solution)
	print("Done\n")
}


// Returns a full path given a relative path to the Nutcracker repository root
function makeFullPath(path)
{
	return filepath(__argv[0]) + "\\..\\" + path
}

// Returns a full path give a relative path to the distribution folder
function distPath(path)
{
	return makeFullPath("tmp\\distribution\\Nutcracker\\") + path
}

function copySquirrel()
{
	copyFiles(
		makeFullPath("interpreters\\squirrel\\"),
		distPath("interpreters\\squirrel\\"),
		[
			"sq.ini",
			"sq.exe",
			"sqdbg.exe"
		])
		
	copyFiles(
		makeFullPath("dependencies\\squirrel\\"),
		distPath("interpreters\\squirrel\\"),
		[
			"COPYRIGHT",
			"HISTORY",
			"README"
		])
		
	copyFiles(
		makeFullPath("dependencies\\squirrel\\doc\\"),
		distPath("interpreters\\squirrel\\doc\\"),
		[
			"sqstdlib3.chm",
			"sqstdlib3.pdf",
			"squirrel3.chm",
			"squirrel3.pdf"
		])
}

function copySquirrelShell()
{
	copyFiles(
		makeFullPath("interpreters\\squirrelsh\\"),
		distPath("interpreters\\squirrelsh\\"),
		[
			"squirrelsh.ini",
			"squirrelsh.exe"
		])
	copyFiles(
		makeFullPath("dependencies\\squirrelsh\\"),
		distPath("interpreters\\squirrelsh\\"),
		[
			"COPYING",
			"COPYING-pcre",
			"README"
		])
		
	copyFiles(
		makeFullPath("dependencies\\squirrelsh\\doc\\"),
		distPath("interpreters\\squirrelsh\\"),
		[
			"squirrelsh.pdf"
		])
}	

function copyNutcracker()
{
	copyFiles(
		makeFullPath("interpreters\\"),
		distPath("interpreters\\"),
		[
			"readme.txt"
		])

	copyFile(
		makeFullPath("bin\\Nutcracker_Win32_Release.exe"),		
		distPath("bin\\Nutcracker.exe"), true)

	copyFiles(
		makeFullPath("bin\\"),
		distPath("bin\\"),
		[
			"config.ini"
		]);	
		
	copyFiles(
		makeFullPath("samples\\"),
		distPath("samples\\"),
		[
			"samples.nws",
			"HelloWorld.nut",
			"Utils.nut"
		]);
}

function build()
{
	buildSolution(makeFullPath("dependencies\\squirrel\\squirrel.sln"),
		"Rebuild", "Win32", "Release")
	buildSolution(makeFullPath("dependencies\\squirrelsh\\squirrelsh.sln"),
		"Rebuild", "Win32", "Release")
	buildSolution(makeFullPath("source\\Nutcracker.sln"),
		"Rebuild", "Win32", "Release");
}

function makeZip()
{
	if (run("c:\\Program Files\\7-Zip\\7z.exe",
		[
			"a",
			"-tzip",
			makeFullPath("tmp\\distribution\\Nutcracker.zip"),
			distPath("")
		]))
	{
		exitWithError("Couldn't create zip file")
	}
}


print("\n")

//
// Build
build()

//
// Delete existing output directory
//rmdir(distPath(""), true)

//
// Copy required files
copyNutcracker()
copySquirrel()
copySquirrelShell()
makeZip()

print("\nDONE\n")
scan()


