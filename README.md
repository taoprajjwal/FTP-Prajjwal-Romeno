# Project 1: File Transfer Protocol

### Run the program

The general format for the execution of both the client and the server is

```
./FTPServer IP-Addrss PortNo.
```

Example including making.

```
make
./FTPServer 127.0.0.1 3333
./FTPClient 127.0.0.1 3333
```

# Commands

#### USER

Sets the username of the user. Should be issued first.

#### Pass

Password for the user. Will prompt an error if given without the USER first.

#### CD

Change directory in the server. Uses chdir() to change the actively working directory on the server side. Accepts relative path syntax (/, ./ etc), but does not accept any bash aliases. (~)

#### LS

Lists the files in the current working directory on the server. LS response is capped to 4096 characters and does not accept paths. Uses dirent.h to retrieve folder information.

#### PWD

Get the current working directory on the server. Each user has their own pwd. Defaults to "/home/cn".

#### !CD

Change the directory on the client.

#### !PWD

Get the current working directory on the client.

#### !LS

List the files in the current working directory on the client.

#### GET

Transfer a file from the server to the client. The file must exist in the current working directory on the server.

#### PUT

Transfer a file from the client to the server. The file must exist in the current working directory on the client.

#### EXIT
Exits the client program. The only way to exit the server is to send a SIGINT through CTRL-C

# Implementation Details

### Concurrency

The program supports up to ten clients at a time. Each connection is represented in the program by a struct which includes information on the username/password, whether or not the user is authenticated, and the directory on the server on which the user is working. There can be multiple connections from the same user/pass combination. Select() is used to ensure concurrency and any files sent are done so on a forked process which terminates after the transfer is complete.

### Authentication.

Username/Password for authentication are stored on the "users.txt" file. Any additional username/password combination need to be written to this file. The first ten lines of this file are read during the beginning of the execution of the program and is stored in a struct in-memory. When authentication is done, it is compared with this in-memory struct. We note that this would not scale well if this FTP Server were to have a lot clients, and a better authentication system would be needed.

### File Transfer

The file transfer mechanism involves connection to a separate file-transfer specific port, which is the port the user has given in the input plus one. The server will fork a new process, through which the file is sent, meanwhile the initial process returns to the listening for new connections on the main port. Concurrency is maintained through file transfer.
