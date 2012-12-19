#ifndef _BINDING_H_
#define _BINDING_H_
#include <v8.h>
#include <node.h>

// #define _DEBUG

using namespace v8;

struct write_req {
  key_t     key;
  int       flag;
  int       value;
  uv_work_t req;
  char *error;
  Persistent<Function> cbl;
};

struct send_req {
  int id;
  int flags;
  char *buffer;
  int buffer_length;
  int ret;
  char *error;
  uv_work_t req;
  Persistent<Function> cbl;
};

struct rcv_req {
  int id;
  int flags;
  int buffer_length;
  char *buffer;
  int ret;
  long msgtyp;
  uv_work_t req;
  char *error;
  Persistent<Function> cbl;
};

Handle<Value> node_msgget(const Arguments&);
Handle<Value> node_msgsnd(const Arguments&);
Handle<Value> node_msgrcv(const Arguments&);
void init(Handle<Object>);
#endif
