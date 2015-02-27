all: libtcp server

server: libtcp socks5 proxy
	gcc -g -o proxy-server -lpthread server.c socks5.o proxy.o tcp.o

socks5:
	gcc -g -c -lpthread -o socks5.o socks5.c

proxy:
	gcc -g -c -lpthread -o proxy.o proxy.c

libtcp:
	gcc -g -c -o tcp.o tcp.c
clean:
	rm -f *.o *~
