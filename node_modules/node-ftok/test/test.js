var expect = require('expect.js'),
    fs = require('fs');

var ftok = require('../');

before(function(done) {
  fs.open(__dirname + '/test', 'w', done);
});

describe('#ftok', function() {
  it('should call ftok when path exists', function(done) {
    ftok(__dirname + '/test', 'b'.charCodeAt(0), function(e, t) {
      expect(e).not.to.be.ok();
      expect(t).to.be.a('number');
      done();
    });
  });

  it('should return an error when path doesn\'t exist', function(done) {
    ftok(__dirname + '/yagxen', 'b'.charCodeAt(0), function(e, t) {
      expect(e).to.be.a('string');
      done();
    });
  });
});


after(function(done) {
  fs.unlink(__dirname + '/test', done);
});
