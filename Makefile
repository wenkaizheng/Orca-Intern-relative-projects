DEBUG = -g -Wall
CFLAGS = `pkg-config libwebsockets sqlite3 --cflags`
LDFLAGS = `pkg-config libwebsockets sqlite3 --libs`
VER = -std=c++11
all: server client

client: client.o utils.o
	g++  $(DEBUG) $(CFLAGS) $(LDFLAGS)  client.o utils.o -o client

server: server.o db.o utils.o
	g++  $(DEBUG) $(CFLAGS) $(LDFLAGS)  server.o db.o utils.o -o server

server.o: server.cpp
	g++  $(VER) -c server.cpp

client.o: client.cpp
	g++ $(VER) -c client.cpp
db.o: db.hpp db.cpp
	g++ $(VER)  -c db.cpp
utils.o: utils.hpp utils.cpp
	g++ $(VER)  -c utils.cpp
.PHONY: clean
clean:
	rm -f *.o
	rm -f server client