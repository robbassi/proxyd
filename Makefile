CC = gcc
FLAGS = -g
DEPS = tcp.h logger.h proxy.h socks5.h server.h
OBJ = tcp.o logger.o proxy.o socks5.o server.o

# build deps
%.o: %.c $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

# link objs
proxyd: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

# wipeout objs and emace files
clean:
	rm -f *.o *~
