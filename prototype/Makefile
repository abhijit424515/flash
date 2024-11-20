all: server client

server: server.cc
	g++ -o server server.cc -lpthread

client: client.cc
	g++ -o client client.cc -lpthread

clean:
	rm -f server client