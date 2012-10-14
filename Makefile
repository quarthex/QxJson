CC = gcc
CFLAGS = -fpic -W -Wall -pedantic -Os -O3 -pipe -DQX_JSON_BUILD
LD = gcc
LDFLAGS = -shared

all: libqxjson.so

clean:
	rm -fv qx.js.value.o;
	rm -fv qx.js.null.o;
	rm -fv qx.js.true.o;
	rm -fv qx.js.false.o;

distclean: clean
	rm -fv libqxjson.so;
	rm -fv test;

test: test.c all
	$(CC) test.c -o test -L. -lqxjson;
	@echo Running test...
	@echo
	@LD_LIBRARY_PATH=. ./test;

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<;

libqxjson.so: qx.js.value.o qx.js.null.o qx.js.true.o qx.js.false.o
	$(LD) $(LDFLAGS) -o $@ $^;

