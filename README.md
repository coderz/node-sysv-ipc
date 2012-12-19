node-sysv-ipc
=============

Node.js async interface to SYS-V IPC

## attach(filename)

Attach msgget to a key/filename (ftok)

Examples:
 ```javascript
 var ipc = new IPC;
 ipc.attach(31337);
 ```

 ```javascript
 var ipc = new IPC;
 ipc.attach("/tmp/file");
 ```

### Params: 

* **String|Number** *filename* or key (key_t)

### Return:

* **undefined** 

## msgsnd(message, callback)

Send a message to a previosuly attached IPC channel

Examples:
 ```javascript
 var ipc = new IPC;
 ipc.attach(31337)
 ipc.msgsnd(new Buffer([0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 41])); // remember, at least 8 bytes
 ```

### Params: 

* **Buffer** *message* 

* **Function** *callback* 

### Return:

* **undefined** 

## msgrcv(length, [OPTIONAL], callback,)

Receive a message

Examples:
 ```javascript
 var ipc = new IPC;
 ipc.attach(31337);
 ipc.msgrcv(30, function(e, b) {
   assert(e, 'msgrcv');
   console.log(b.toString());
 });
 ```

### Params: 

* **Number** *length* of the message

* **Number** *[OPTIONAL]* mtype

* **Function** *callback,* called with [error, msgBuffer]

### Return:

* **undefined** 
