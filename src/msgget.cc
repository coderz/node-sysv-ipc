#include <iostream>
#include <string>
#include <cstdio>
#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "node_buffer.h"
#include "binding.h"

using namespace v8;

void msgget_async(uv_work_t *req) {
  struct write_req *orig = (struct write_req *) req->data;

  orig->value = msgget(orig->key, orig->flag);
}

void after_mssget_async(uv_work_t *req) {
  struct write_req *orig = (struct write_req *) req->data;
  Handle<Value> err =
    (orig->value < 0) ? String::New(strerror(errno)) : Null();

  Handle<Value> argv[] = { err, Number::New((int) orig->value) };
  orig->cbl->Call(Context::GetCurrent()->Global(), 2, argv);
}

Handle<Value> node_msgget(const Arguments& args) {
  HandleScope scope;
  struct write_req *req = new write_req;

  if(
      args.Length() < 2 || 
      !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsFunction()
    ) {
    ThrowException(Exception::TypeError(String::New("msgget requires 3 arguments")));
    return scope.Close(Undefined());
  }
  
  req->key  = (key_t) args[0]->ToNumber()->Value();
  req->flag = (int) args[1]->ToNumber()->Value(); 
  req->cbl  = Persistent<Function>::New(Local<Function>::Cast(args[2]));
  req->req.data = req;

  uv_queue_work(uv_default_loop(), &req->req, msgget_async, after_mssget_async);

  return scope.Close(Undefined());
}

