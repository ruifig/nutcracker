local i=0;
local anotherArray = [10,20,30]

function testWatch(a)
{
	return "Hello " + a;
}


function foo1(a)
{
	local localArray = [1,2,3,4];
	print(pow(2,8) + "\n");
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
while(1)
{
	print("Hello " + i + "\n");
	i++;
}
