function foo1(a)
{
	return a;
}

function foo2(a)
{
	return foo1(a);
}

function foo3(a)
{
	return foo2(a);
}


function bar1(a)
{
	return a;
}

function bar2(a)
{
	local r = bar1(a);
	return r;
}

function bar3(a)
{
	local r = bar2(a);
	return r;
}


foo3(0);
bar3(0);
