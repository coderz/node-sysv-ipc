#include <v8.h>
#include <node.h>

#include "binding.h"

using namespace v8;

void init(Handle<Object> target) {
  target->Set(
    String::NewSymbol("msgget"),
    FunctionTemplate::New(node_msgget)->GetFunction()
  );

  target->Set(
    String::NewSymbol("msgsbd"),
    FunctionTemplate::New(node_msgsnd)->GetFunction()
  );
}

NODE_MODULE(msgget, init)
