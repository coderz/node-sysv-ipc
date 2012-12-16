var msgget = require('../'),
    ftok   = require('node-ftok'),
    expect = require('expect.js');

before(function(done) {
  require('fs').open(__dirname + '/test', 'w', done);
});

describe('#msgget', function() {
  it('should call msgget when id exists', function(done) {
    ftok('test', 'b'.charCodeAt(0), function(e, n) {
      msgget(n, 512, function(e, x) {
        expect(x > 0).to.be.ok();
        done();
      });
    });
  });

  it('should fail with an error if wrong id', function(done) {
    msgget('asd', 512, function(e, k) {
      expect(e).to.be.ok();
      done();
    });
  });
});
