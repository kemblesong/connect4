# connect4
### A client/server command line game written in C   

## Setup
`make client` makes the client executable  
`make server` makes the server executable
`make all` makes both the client and server executables  

## Usage
On the server side, choose an open port and run:  
`./server port` where `port` is the port number.

If you are running the client on the same network as the server:
`./client localhost port` where `port` is the port number you chose just now.  
Otherwise, you can replace `localhost` with the domain your server is on.  

Now play a game of connect4 with the computer!  

## Notes
The server can handle multiple client requests concurrently through the use
of multithreading. Try it out!  
A log.txt file containing all server interactions is automatically written as
you run the program.
