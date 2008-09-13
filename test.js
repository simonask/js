#!./js

load("prelude.js");

var str = "";
while (!IO.stdin.eof)
{
	str += IO.stdin.read(1024);
}
println(str);

println(str.constructor.name);

println(false && "HEJSA");
