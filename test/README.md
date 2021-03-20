# Tests

## Generic tests
The set of commands were used to test the basic functionality of the all the commands are provided in "test-basic.txt". Please not that you will need to download the auxiliary test files in this folder and place it in "/home/cn" for GET/PUT to work. The things tested are:

- Wrong Username/Password combinations, i.e PASS called before USER.
- Authentication error on using other commands (CD,LS etc) before authentication
- CD works with relative paths
- GET, PUT works with different types of files

## Concurrency tests
In order to ensure concurrency, we performed tests using two or more connections, each one done by running ./FTPClient  on an.These are in the "test-conc.txt" file. Please note that the separator ("-") is used as an indicator to indicate that these commands were done from a different terminal. The test run were:
- Multiple clients connect using different usernames.
- Multiple clients connect using the same username.
- Users change the PWD and CD without effecting each other.
- A client tries to connect while a file transfer is going.
