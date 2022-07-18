CC=gcc
MAIN=main.o
SERVER=server.o
FLAGS=-g

all: server main
server: $(SERVER)
	$(CC) $(FLAGS) -o server $(SERVER)
main: $(MAIN)
	$(CC) $(FLAGS) -o main $(MAIN)
$(SERVER): server.c 
	$(CC) $(FLAGS) -c server.c 
$(MAIN): main.c 
	$(CC) $(FLAGS) -c main.c 

.PHONY: clean all
clean:
	rm -f *.o main server