var expect = require('expect.js');

var IPC = require('../').IPC;

before(function(done) {
  require('fs').open(__dirname + '/test', 'w', done);
});

var ipc = new IPC;
var ipc2 = new IPC;

describe('attach', function() {
  it('should attach to a file via ftok', function(done) {
    ipc.attach(__dirname + '/test');
    ipc2.attach(__dirname + '/test');

    ipc.buffer.flush(function() {
      ipc2.buffer.flush(function() {
        expect(ipc.msgqid).to.be.a('number');
        expect(ipc2.msgqid).to.be.a('number');
        done();
      });
    });
  });
});

describe('#msgsnd/msgrcv', function() {
  it('should send/receive message', function(done) {
    ipc2.msgrcv(9, function(e, msg) {
      expect(e).not.to.be.ok();
      expect(msg[8]).to.be(0x41);
      done();
    });

    ipc.msgsnd(new Buffer([0, 0, 0, 0, 0, 0, 0, 1, 0x41]), function(e) {
      expect(e).not.to.be.ok();
    });
  });
});
