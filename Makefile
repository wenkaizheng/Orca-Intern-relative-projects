DEBUG = -g -Wall
CFLAGS = `pkg-config libwebsockets sqlite3 --cflags`
LDFLAGS = `pkg-config libwebsockets sqlite3 --libs`
VER = -std=c++11
all: server client client_t server_t

client: client.o utils.o
	g++  $(DEBUG) $(CFLAGS) $(LDFLAGS)  client.o utils.o -o client

server: server.o db.o utils.o
	g++  $(DEBUG) $(CFLAGS) $(LDFLAGS)  server.o db.o utils.o -o server

client_t: client_t.o utils.o
	g++ $(DEBUG) $(CFLAGS) $(LDFLAGS) client_t.o  utils.o  -o client_t
client_t.o: client_t.cpp
	g++  $(VER) -c client_t.cpp

server_t: server_t.o utils.o
	g++ $(DEBUG) $(CFLAGS) $(LDFLAGS) server_t.o utils.o -o server_t
server_t.o: server_t.cpp
	g++  $(VER) -c server_t.cpp
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
	rm -f server client server_t client_t