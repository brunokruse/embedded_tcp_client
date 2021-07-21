# build our client executable from client.c
all: client.c
	gcc -g -Wall -o client client.c

clean:
	$(RM) client