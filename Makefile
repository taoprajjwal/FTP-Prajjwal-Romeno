FTPClient: FTPClient.c
	gcc -w FTPClient.c -o FTPClient

FTPServer: FTPServer.c
	gcc -w FTPServer.c -o FTPServer

all: FTPClient FTPServer

clean:
	rm *.o FTPclient FTPServer