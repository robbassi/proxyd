all: libtcp server

server: libtcp socks5 proxy
	gcc  -o proxy-server -lpthread server.c socks5.o proxy.o tcp.o

socks5:
	gcc -c -lpthread -o socks5.o socks5.c

proxy:
	gcc -c -lpthread -o proxy.o proxy.c

libtcp:
	gcc -c -o tcp.o tcp.c
clean:
	rm -f *.o *~
