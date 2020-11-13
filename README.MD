#                     XXX====Server Nance====XXX

The client requests file(s) from the server and downloads the file into its own directory.

## Running the Program

Run the client and server in two different directories.

Open 2 terminals simultaneously,one for server ,another for client.
 
#### Running the server
```
gcc server.c -o server
./server
```
#### Running the client
```
gcc client.c -o client
./client
```

## Usage

The client supports the following commands :

#### Supported Commands:

1.get - request a file from server
```
>get <filename>		
>get <filename1> <filename2> ...
```
2.exit - Disconnects from server and terminates client program
```
>exit
```
## Error Messages
The client side interface throws error messages (colored red) in the following cases:

* When the client requests a file not present in the server directory
* When the client requests a file which cannot be opened in the server directory(eg: due to permissions)
* When the client requests an empty file from the server
* When any command other than ```get``` /```exit``` used
* When no file name is provided after ```get```

##### The Server gives segmentation fault if client is closed abruptly without the exit command(eg: Ctrl+C)

The server side interface also provides helpful messages to keep track of the entire file transfer process and related errors.
