var msgget = require('../').native,
    ftok   = require('node-ftok'),
    expect = require('expect.js');

var id;

before(function(done) {
  require('fs').open(__dirname + '/test', 'w', done);
});

describe('#msgget', function() {
  it('should call msgget when id exists', function(done) {
    ftok(__dirname + '/test', 'b'.charCodeAt(0), function(e, n) {
      expect(n > -1).to.be.ok();
      msgget.msgget(n, 950, function(e, x) { // 512 = IPC_CREAT
        expect(e).not.to.be.ok();
        expect(x !== -1).to.be.ok();
        id = x;
        
        done();
      });
    });
  });

  it('should fail with an error if wrong id', function(done) {
    msgget.msgget(31337, 1024, function(e, k) { // 1024 = IPC_EXCL
      expect(e).to.be.a(Error);
      done();
    });
  });
});

describe('#msgsnd/msgrcv', function() {
  it('should send/receive messages', function(done) {
    msgget.msgrcv(id, 14, 0, 0, function(e, b) {
      b = b.slice(8);
      
      expect(e).not.to.be.ok();
      expect(b).to.be.a(Buffer);
      expect(b.toString()).to.be('nexgay');
      done();
    });
    
    var b = new Buffer(8 + 6);
    // set mtype to 1
    b.writeInt32LE(0, 0)
    b.writeInt32LE(1, 4);
    // -->
    b.write('nexgay', 8);

    msgget.msgsnd(id, b, 0, function(e) {
      expect(e).not.to.be.ok();
    });
  });
});
