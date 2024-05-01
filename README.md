# C++ socket example
Example on how to use sockets in C++ 

- use ```make client``` and ```make server``` to build
- ```./server_test``` to run the server
- ```./client_test``` to run the client

One could connect to the server also using netcat```nc 127.0.0.1 8080``` altought not recommended.
The purpose it is to connect from multiple clients and replicate a chat server. 

A client can close the connection by sending ```!q``` as a message.
This way server will handle the closing connection properly.
The code might be improved by introducing a 'TIME_OUT' for clients which are not active for a while.


