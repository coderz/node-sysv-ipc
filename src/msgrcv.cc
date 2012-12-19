#include <node.h>
#include <v8.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "node_buffer.h"
#include "binding.h"

using namespace v8;
using namespace node;

void msgrcv_async(uv_work_t *req) {
  struct rcv_req *orig = (struct rcv_req *) req->data;

  #ifdef _DEBUG
  std::cout << "-- MSGRCV --\n" << "ID: " << orig->id << "\nBUFFER LENGTH: " << orig->buffer_length << "\nMSGTYP: " << orig->msgtyp << "\nFLAGS: " << orig->flags << std::endl;
  #endif
    

  orig->ret = msgrcv(orig->id, orig->buffer, orig->buffer_length, orig->msgtyp, orig->flags);

  if (orig->ret < 1) {
    orig->error = strerror(errno);
    #ifdef _DEBUG
    std::cout << "ERRNO: " << errno << std::endl;
    #endif
  }
}

void after_msgrcv_async(uv_work_t *req) {
  struct rcv_req *orig = (struct rcv_req *) req->data;
  Handle<Value> err =
    (orig->ret < 0) ? String::New(strerror(errno)) : Null();

  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(global->Get(String::New("Buffer")));
  
  Buffer *slowMsg = Buffer::New(orig->buffer_length);
  memcpy(Buffer::Data(slowMsg), orig->buffer, orig->buffer_length);

  Handle<Value> bufArgv[] = { slowMsg->handle_, Integer::New(orig->buffer_length), Integer::New(0) };
  
  Handle<Value> msg;

  if (orig->ret < 0)
    msg = Null();
  else
    msg = bufferConstructor->NewInstance(3, bufArgv);

  Handle<Value> argv[] = { err, msg };
  orig->cbl->Call(Context::GetCurrent()->Global(), 2, argv);
}

Handle<Value> node_msgrcv(const Arguments& args) {
  HandleScope scope;
  struct rcv_req *req = new rcv_req;
  
  if(
      args.Length() < 5 ||
      !args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber() || !args[3]->IsNumber() || !args[4]->IsFunction()
    ) {
    ThrowException(Exception::TypeError(String::New("msgrcv requires 3 arguments")));

    return scope.Close(Undefined());
  }

  req->id  = args[0]->ToNumber()->Value();
  req->buffer_length = args[1]->ToNumber()->Value();
  req->flags = args[2]->ToNumber()->Value();
  req->msgtyp = args[3]->ToNumber()->Value();
  req->buffer = new char[req->buffer_length];
  req->cbl  = Persistent<Function>::New(Local<Function>::Cast(args[4]));
  req->req.data = req;

  uv_queue_work(uv_default_loop(), &req->req, msgrcv_async, after_msgrcv_async);

  return scope.Close(Undefined());
}

