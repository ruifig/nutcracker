
/*!
Terminates the program, displaying the specified message
*/
function exitWithError(msg)
{
	local msg = format("ERROR: %s\n", msg);
	error(msg);
	exit(1)
}
