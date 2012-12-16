#include <iostream>
#include <string>
#include <cstdio>
#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

#include "node_buffer.h"

using namespace v8;

struct write_req {
  std::string *path;
  int id;
  int k;
  Persistent<Function> cbl;
  uv_work_t req;
};

void ftok_async(uv_work_t *req) {
  struct write_req *original_req = (struct write_req *) req->data;
  const char *path = original_req->path->c_str();
  key_t k = ftok(path, original_req->id);

  original_req->k = k;
}

void after_ftok_async(uv_work_t *req) {
  struct write_req *original_req = (struct write_req *) req->data;
  Handle<Value> err = 
    (original_req->k < 0) ?
      String::New(strerror(errno)) :
      Null();

  Handle<Value> argv[] = { err, Number::New((int) original_req->k) };
  original_req->cbl->Call(Context::GetCurrent()->Global(), 2, argv);
}

Handle<Value> node_ftok(const Arguments& args) {
  HandleScope scope;
  struct write_req *req = new write_req;
  
  if (args.Length() < 3) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }
  
  if (!args[0]->IsString() || !args[1]->IsNumber() || !args[2]->IsFunction()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }
  
  String::AsciiValue astr(args[0]->ToString());
  std::string *ll = new std::string(*astr);

  req->path = ll;
  req->id = (int) args[1]->ToNumber()->Value();
  req->cbl = Persistent<Function>::New(Local<Function>::Cast(args[2]));
  req->req.data = req;

  uv_queue_work(uv_default_loop(), &req->req, ftok_async, after_ftok_async);

  return scope.Close(Undefined());
}

void init(Handle<Object> target) {
  target->Set(String::NewSymbol("ftok"),
      FunctionTemplate::New(node_ftok)->GetFunction());
}

NODE_MODULE(ftok, init)
