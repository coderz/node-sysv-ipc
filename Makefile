.PHONY: docs

all:
	node-gyp build
test:
	node test.js
configure:
	node-gyp configure

docs:
	mkdir -p docs
	dox-foundation -t node-sysv-ipc -T docs -s lib
docs-md:
	markdox lib/ipc.js -o docs.md

