
function derp(what, i)
{
	print(what)
	while(true)
	{
		print("Hello World!" + i++ + "\n")
	}
}

function foo(what)
{
	derp(what, 0)
}

local msg = "Hello World"
foo(msg)
