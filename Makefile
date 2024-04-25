server:
	g++ -std=c++0x -pthread server.cpp -o server_test
client:
	g++ -std=c++0x -pthread client.cpp -o client_test
