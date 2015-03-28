all: libtcp server

server: libtcp socks5 proxy logger
	gcc -g -o proxy-server server.c socks5.o proxy.o tcp.o logger.o

socks5:
	gcc -g -c -o socks5.o socks5.c

proxy: 
	gcc -g -c -o proxy.o proxy.c

libtcp:
	gcc -g -c -o tcp.o tcp.c

logger:
	gcc -g -c -o logger.o logger.c

clean:
	rm -f *.o *~
