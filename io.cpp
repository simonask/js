#include "io.h"
using namespace v8;

#include <stdio.h>
#include <string.h>

#include <assert.h>

static Persistent<FunctionTemplate> io_templ;

enum InternalFields { FILE_HANDLE = 0, NUM_FIELDS };

static Handle<Value> wrap_file_pointer(FILE* fp)
{
	Local<Object> io = io_templ->GetFunction()->NewInstance();
	io->SetInternalField(FILE_HANDLE, External::New((void*)fp));
	return io;
}

static FILE* unwrap_file_pointer(Handle<Object> self)
{
	Local<Value> field = self->GetInternalField(FILE_HANDLE);
	return reinterpret_cast<FILE*>(Handle<External>::Cast(field)->Value());
}

/*
	METHODS
*/
static Handle<Value> js_io_new(const Arguments& args)
{
	if (args.Length() < 2 && args.Length() != 0)
		assert(0 && "not enough arguments for js_io_new!"); // TODO: Throw exception
	FILE* fp = fdopen(args[0]->ToInteger()->Value(), *String::AsciiValue(args[1]));
	args.This()->SetInternalField(FILE_HANDLE, External::New(fp));
	return args.This();
}

static Handle<Value> js_io_open(const Arguments& args)
{
	if (args.Length() < 1)
		assert(0 && "not enough arguments for js_io_open!"); // TODO: Throw exception
	const char* flags = args.Length() < 2 ? "rw" : *String::AsciiValue(args[1]);
		
	FILE* fp = fopen(*String::Utf8Value(args[0]), flags);
	if (!fp)
		return Undefined();
	else
		return wrap_file_pointer(fp);
}

static Handle<Value> js_io_close(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	fclose(fp);
	return True();
}

static Handle<Value> js_io_read(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	size_t n = args[0]->ToInteger()->Value();
	char buf[n];
	size_t m = fread(buf, 1, n, fp);
	return String::New(buf, m);
}

static Handle<Value> js_io_write(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	const char* data = *String::Utf8Value(args[0]);
	size_t n = fwrite(data, 1, strlen(data), fp);
	return Integer::New(n);
}

static Handle<Value> js_io_flush(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	return fflush(fp) ? False() : True();
}

static Handle<Value> js_io_seek(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	int offset = args[0]->ToInteger()->Value();
	int whence = args[1]->ToInteger()->Value();
	return fseek(fp, offset, whence) ? False() : True();
}

static Handle<Value> js_io_tell(const Arguments& args)
{
	FILE* fp = unwrap_file_pointer(args.This());
	return Integer::New(ftell(fp));
}

/*
	ACCESSORS
*/

static Handle<Value> js_io_handle(Local<String> property, const AccessorInfo& info)
{
	return Integer::New((int64_t)unwrap_file_pointer(info.Holder()));
}

static Handle<Value> js_io_eof(Local<String> property, const AccessorInfo& info)
{
	FILE* fp = unwrap_file_pointer(info.Holder());
	return feof(fp) ? True() : False();
}


static void dummy_setter(Local<String>, const Local<Value>, const AccessorInfo&) {}

void IO::initialize(Handle<Object>& global)
{
	io_templ = Persistent<FunctionTemplate>(FunctionTemplate::New(js_io_new));
	io_templ->SetClassName(String::New("IO"));
	Local<ObjectTemplate> ot = io_templ->InstanceTemplate();
	ot->SetInternalFieldCount(NUM_FIELDS);
	ot->SetAccessor(String::New("handle"), js_io_handle, dummy_setter);
	ot->SetAccessor(String::New("eof"), js_io_eof, dummy_setter);
	ot->Set(String::New("read"), FunctionTemplate::New(js_io_read));
	ot->Set(String::New("write"), FunctionTemplate::New(js_io_write));
	ot->Set(String::New("close"), FunctionTemplate::New(js_io_close));
	ot->Set(String::New("flush"), FunctionTemplate::New(js_io_flush));
	ot->Set(String::New("seek"), FunctionTemplate::New(js_io_seek));
	ot->Set(String::New("tell"), FunctionTemplate::New(js_io_tell));
	
	io_templ->Set(String::New("open"), FunctionTemplate::New(js_io_open));
	Local<Object> io = io_templ->GetFunction();
	io->Set(String::New("stdout"), wrap_file_pointer(stdout));
	io->Set(String::New("stdin"), wrap_file_pointer(stdin));
	io->Set(String::New("stderr"), wrap_file_pointer(stderr));
	
	global->Set(String::New("IO"), io);
}
