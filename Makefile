all:
	node-gyp build
test:
	node test.js
configure:
	node-gyp configure
