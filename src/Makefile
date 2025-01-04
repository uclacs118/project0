CC=gcc
CPPFLAGS=-Wall -Wextra 
LDFLAGS=
LDLIBS=-lsocket

DEPS=

all: libsocket.a server client 

server: server.o $(DEPS)
client: client.o $(DEPS)
libsocket.a: libsocket.c
	$(CC) -shared -fPIC $(CPPFLAGS) -o libsocket.a libsocket.c

clean:
	@find . -type f \
		! -name "*.c" \
		! -name "*.h" \
		! -name "*.cpp" \
		! -name "*.hpp" \
		! -name "Makefile" -delete
	@find . -type d \( ! -name "." \) -exec rm -rf {} +
