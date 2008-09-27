#include "process.h"
using namespace v8;

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

static Persistent<FunctionTemplate> process_templ;

static Handle<Value> wrap_pid(pid_t pid)
{
	Local<Object> process = process_templ->GetFunction()->NewInstance();
	process->Set(String::New("pid"), Integer::New(pid));
	return process;
}

pid_t unwrap_pid(Handle<Object> process)
{
	pid_t pid = (pid_t)process->Get(String::New("pid"))->ToInteger()->Value();
	return pid;
}

static Handle<Value> js_process_new(const Arguments& args)
{
	return Undefined();
}

static Handle<Value> js_process_fork(const Arguments& args)
{
	pid_t pid = fork();
	if (pid)
		return wrap_pid(pid);
	else
		return Undefined();
}

static Handle<Value> js_process_exit(const Arguments& args)
{
	int status;
	if (args.Length() > 0)
		status = args[0]->ToInteger()->Value();
	else
		status = 0;
	exit(status);
}

static Handle<Value> js_process_wait(const Arguments& args)
{
	pid_t pid = unwrap_pid(args.This());
	int status;
	waitpid(pid, &status, 0);
	return Integer::New(WEXITSTATUS(status));	// TODO: Support signals etc.
}

static void dummy_setter(Local<String>, const Local<Value>, const AccessorInfo&) {}

void Process::initialize(Handle<Object>& global)
{
	process_templ = Persistent<FunctionTemplate>(FunctionTemplate::New(js_process_new));
	process_templ->SetClassName(String::New("Process"));
	Local<ObjectTemplate> ot = process_templ->InstanceTemplate();
	process_templ->Set(String::New("fork"), FunctionTemplate::New(js_process_fork));
	process_templ->Set(String::New("exit"), FunctionTemplate::New(js_process_exit));
	ot->Set(String::New("wait"), FunctionTemplate::New(js_process_wait));
	Local<Object> process = process_templ->GetFunction();
	global->Set(String::New("Process"), process);
}
