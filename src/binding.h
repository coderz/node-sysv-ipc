#ifndef _BINDING_H_
#define _BINDING_H_
#include <v8.h>
#include <node.h>

using namespace v8;

struct write_req {
  key_t     key;
  int       flag;
  int       value;
  uv_work_t req;
  Persistent<Function> cbl;
};

Handle<Value> node_msgget(const Arguments&);
void init(Handle<Object>);
#endif
