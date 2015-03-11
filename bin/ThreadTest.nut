function do2(a)
{
	print(a);
	::suspend("s3");
	return 10;
}

function do1(a,b)
{
	local c;
	c = a + b;
	::suspend("s2");
	r = do2(c);
	return c;
}

function coroutine_test(a,b)
{
	::suspend("s1");
	local r = do1(a,b);
	return r;
}

local coro = ::newthread(coroutine_test);


local susparam = coro.call("Hello", " World!");

local i=1;
do
{
	::print("suspend passed (" + susparam + ")\n");
	susparam = coro.wakeup();	
} while(coro.getstatus()=="suspended")
