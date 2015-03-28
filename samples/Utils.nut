
// Spins for the specified time
function pause(seconds)
{
	local start = clock();
	local end = start + seconds;
	while (clock()<end)
	{
	}
}

