#include "v8.h"
using namespace v8;

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#include "io.h"
#include "process.h"

Handle<Value> LoadScript(const char* file)
{
	ifstream fs(file);
	if (!fs.is_open())
	{
		cerr << "ERROR: File not found: " << file << endl;
		return Undefined();
	}
	stringstream source_str;
	string line;
	while (fs.is_open() && !fs.eof())
	{
		getline(fs, line);
		if (line[0] != '#')
			source_str << line << endl;
	}
	fs.close();
	
	Handle<String> source = String::New(source_str.str().c_str());
	Handle<Script> script = Script::Compile(source);
	return script->Run();
}

Handle<Value> js_load(const Arguments& args)
{
	const char* file = *String::AsciiValue(args[0]);
	return LoadScript(file);
}

int main (int argc, char const *argv[])
{
	HandleScope handle_scope;
	
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	
	global->Set(String::New("load"), FunctionTemplate::New(js_load));
	Persistent<Context> context = Context::New(NULL, global);
	Context::Scope context_scope(context);
	Handle<Object> global_obj(context->Global());
	IO::initialize(global_obj);
	Process::initialize(global_obj);
	
	Handle<Value> result = LoadScript(argv[1]);
	
	context.Dispose();
	
	return 0;
}