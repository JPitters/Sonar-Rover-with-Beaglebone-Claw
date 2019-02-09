#ifndef WINSOCK2_H
#define WINSOCK2_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <thread>
#include <chrono>


enum SocketType{ CLIENT = 1, SERVER };
enum ConnectionType { TCP = 1 , UDP };
const int DEFAULT_SIZE = 128; 

class MySocket 
{	
protected: 
	char * Buffer; 
	SOCKET WelcomeSocket , ConnectionSocket; 
	struct sockaddr_in SvrAddr; 
	SocketType mySocket; 
	std::string IPAddr; 
	int Port; 
	ConnectionType connectionType; 
	bool bTCPConnect; 
	int maxSize; 
	WSADATA wsa_data;
public:
	MySocket(SocketType, std::string, unsigned int , ConnectionType, unsigned int);
	~MySocket();
	void setPort(int); //not done yet
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);
	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char*, int);
	int GetData(char*);
	void start_DLLS();
	SOCKET initialize_tcp_socket();
	SOCKET initialize_udp_socket();
	void server_bind_socket();
	void server_listen_socket();
	void client_connect_to_tcp_server();
};

#endif WINSOCK_H
