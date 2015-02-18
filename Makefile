all: libtcp server

server: libtcp socks5
	gcc -o proxy-server server.c socks5.o tcp.o

socks5:
	gcc -c -o socks5.o socks5.c

libtcp:
	gcc -c -o tcp.o tcp.c
