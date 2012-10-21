CC = gcc
CFLAGS = -fpic -W -Wall -pedantic -Os -O3 -pipe -DQX_JSON_BUILD
LD = gcc
LDFLAGS = -shared

SOURCES = \
	qx.json.value.c \
	qx.json.null.c \
	qx.json.true.c \
	qx.json.false.c \
	qx.json.number.c \
	qx.json.string.c
OBJECTS = $(SOURCES:.c=.o)

all: libqxjson.so

clean:
	rm -fv $(OBJECTS);

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

libqxjson.so: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^;

