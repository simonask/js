#!./js

load("prelude.js");

var pid = Process.fork();
println("hello from " + (pid ? pid.pid : 0));
if (pid)
{
	var result = pid.wait();
	println("return with result: " + result);
}
else
{
	Process.exit(123);
}
