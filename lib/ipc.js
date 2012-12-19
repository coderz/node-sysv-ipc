var assert = require('assert'),
    ftok = require('node-ftok'),
    sysv = require('../').native,
    buffy = require('node-buffy');

var native = require('../').native;

function IPC() {
  this.msgqid = null;
  this.key = null;
  this.buffer = buffy();
}

/**
 * Attach msgget to a key/filename (ftok)
 *
 * Examples:
 *  ```javascript
 *  var ipc = new IPC;
 *  ipc.attach(31337);
 *  ```
 *
 *  ```javascript
 *  var ipc = new IPC;
 *  ipc.attach("/tmp/file");
 *  ```
 *
 * @param {String/Number} filename or key (key_t)
 * @return undefined
 * @api public
 *
 */

IPC.prototype.attach = function(o) {
  assert(
    typeof o === 'number' || 
    (typeof o === 'string' && require('fs').existsSync(o)),
      'Argument must be a number or an actual filename'
  );

  var fn = typeof o === 'string' ?
    ftok : function(o, b, cbl) {
      cbl(null, o);
    };

  var self = this;

  this.buffer.buffer(function(cbl) {
    fn(o, 'b'.charCodeAt(0), function(e, key) {
      assert(!e, 'Error in ftok');
      
      self.key = key;

      sysv.msgget(key, 950, function(e, msgqid) {
        assert(!e, 'Error in msgget');

        self.msgqid = msgqid;
        cbl();
      });
    });
  });
};

/**
 * Send a message to a previosuly attached IPC channel
 *
 * Examples:
 *  ```javascript
 *  var ipc = new IPC;
 *  ipc.attach(31337)
 *  ipc.msgsnd(new Buffer([0, 0, 0, 0, 0, 0, 0, 0, 41, 41, 41])); // remember, at least 8 bytes
 *  ```
 *
 * @param {Buffer} message
 * @param {Function} callback
 * @return undefined
 * @api public
 */

IPC.prototype.msgsnd = function(msg, cbl) {
  var self = this;

  this.buffer.flush(function() {
    sysv.msgsnd(self.msgqid, msg, 0, cbl || function() {});
  });
};

/**
 * Receive a message
 *
 * Examples:
 *  ```javascript
 *  var ipc = new IPC;
 *  ipc.attach(31337);
 *  ipc.msgrcv(30, function(e, b) {
 *    assert(e, 'msgrcv');
 *    console.log(b.toString());
 *  });
 *  ```
 *
 * @param {Number} length of the message
 * @param {Number} [OPTIONAL] mtype
 * @param {Function} callback, called with [error, msgBuffer]
 * @return undefined
 * @api public
 */

IPC.prototype.msgrcv = function() {
  var self = this,
      args = [].slice.call(arguments),
      length = args.shift(),
      cbl = args.pop(),
      mtype = args.shift();

  this.buffer.flush(function() {
    sysv.msgrcv(self.msgqid, length, mtype || 0, 0, cbl);
  });
};

module.exports = IPC;
