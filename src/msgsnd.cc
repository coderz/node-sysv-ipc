#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <iostream>
#include <string>

#include "node_buffer.h"
#include "binding.h"

using namespace v8;
using namespace node;

void msgsnd_async(uv_work_t *req) {
  struct send_req *orig = (struct send_req *) req->data;

  orig->ret = msgsnd(orig->id, orig->buffer, orig->buffer_length, orig->flags);

  if (orig->ret < 1) {
    orig->error = strerror(errno);
    #ifdef _DEBUG
    std::cout << "ERRNO: " << errno << std::endl;
    #endif
  }

  #ifdef _DEBUG
  std::cout << "-- MSGSND --\n" << "ID: " << orig->id << "\nBUFFER LENGTH: " << orig->buffer_length <<  "\nFLAGS: " << orig->flags << std::endl;
  #endif
}

void after_msgsnd_async(uv_work_t *req) {
  struct send_req *orig = (struct send_req *) req->data;
  Handle<Value> err =
    (orig->ret < 0) ? String::New(orig->error) : Null();

  Handle<Value> argv[] = { err, Number::New((int) orig->ret) };
  orig->cbl->Call(Context::GetCurrent()->Global(), 2, argv);
}

Handle<Value> node_msgsnd(const Arguments& args) {
  HandleScope scope;
  struct send_req *req = new send_req;

  if(
      args.Length() < 3 || 
      !args[0]->IsNumber() || !Buffer::HasInstance(args[1]->ToObject()) || !args[2]->IsNumber() || !args[3]->IsFunction()
    ) {
    ThrowException(Exception::TypeError(String::New("msgsnd requires 3 arguments")));
    return scope.Close(Undefined());
  }

  req->id  = args[0]->ToNumber()->Value();
  req->buffer = Buffer::Data(args[1]->ToObject());
  req->buffer_length = Buffer::Length(args[1]->ToObject());
  req->flags = args[2]->ToNumber()->Value(); 
  req->cbl  = Persistent<Function>::New(Local<Function>::Cast(args[3]));
  req->req.data = req;

  uv_queue_work(uv_default_loop(), &req->req, msgsnd_async, after_msgsnd_async);

  return scope.Close(Undefined());
}

