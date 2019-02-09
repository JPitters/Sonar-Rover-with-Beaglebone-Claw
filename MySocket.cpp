#include "MySocket.h"

MySocket::MySocket(SocketType socketType, std::string ipaddress, unsigned int port, ConnectionType connectType, unsigned int bufferSize)
{
	this->mySocket = socketType; 
	this->IPAddr = ipaddress; 
	this->Port = port; 
	this->connectionType = connectType; 

	if (bufferSize == 0)
	{
		this->maxSize = DEFAULT_SIZE;
	}
	else 
	{
		this->maxSize = bufferSize;

	}
	Buffer = new char[maxSize];
	if (this->mySocket == SERVER)
	{
		this->start_DLLS();
		if (this->connectionType == TCP)
		{
			this->WelcomeSocket = this->initialize_tcp_socket();
			this->server_bind_socket();
			this->server_listen_socket();
			this->ConnectTCP();
		}
		else if (this->connectionType == UDP)
		{
			this->WelcomeSocket = this->initialize_udp_socket();
			this->server_bind_socket();
		}
	}
	else if (this->mySocket == CLIENT)
	{
		this->start_DLLS();
		if (this->connectionType == TCP)
		{
			this->WelcomeSocket = this->initialize_tcp_socket();
			// this->ConnectTCP();
		}
		else if (this->connectionType == UDP)
		{
			this->WelcomeSocket = this->initialize_udp_socket();
		}
	}
	
}

void MySocket::ConnectTCP()
{
	if (this->mySocket == SERVER)
	{
		if ((this->ConnectionSocket = accept(this->WelcomeSocket, NULL, NULL)) == SOCKET_ERROR) {
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "Could not accept incoming connection." << std::endl;
			std::cin.get();
			exit(0);
		}
		else {
			std::cout << "Connection Accepted" << std::endl;
			this->bTCPConnect = true;
		}
	}
	else if (this->mySocket == CLIENT)
	{
		struct sockaddr_in SvrAddr;
		SvrAddr.sin_family = AF_INET; //Address family type internet
		SvrAddr.sin_port = htons(this->Port); //port (host to network conversion)
		SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address
		if ((connect(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(this->WelcomeSocket);
			WSACleanup();
			std::cout << "Could not connect to the server" << std::endl;
			std::cin.get();
			exit(0);
		}
		else {
			std::cout << "Server Accepted Connection" << std::endl;
			this->bTCPConnect = true;
		}
	}
}

void MySocket::DisconnectTCP()
{
	if (this->mySocket == SERVER)
	{
		closesocket(this->WelcomeSocket);
		closesocket(this->ConnectionSocket);
	}
	else if (this->mySocket == CLIENT)
	{
		closesocket(this->WelcomeSocket);
	}
}

MySocket::~MySocket()
{
	closesocket(this->WelcomeSocket); 
	closesocket(this->ConnectionSocket);
}

void MySocket::setPort(int port)
{
	if (this->bTCPConnect)
		std::cerr << "Connection has already been established!";
	else
		this->Port = port;
}

int MySocket::GetPort()
{
	return this->Port;
}

void MySocket::SetType(SocketType theType)
{
	this->mySocket = theType;
}
SocketType MySocket::GetType()
{
	return this->mySocket;
}

void MySocket::start_DLLS() {
	if ((WSAStartup(MAKEWORD(2, 2), &this->wsa_data)) != 0) {
		std::cout << "Could not start DLLs" << std::endl;
		std::cin.get();
		exit(0);
	}
}

//initializes a socket and returns it
SOCKET MySocket::initialize_tcp_socket() {
	SOCKET LocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (LocalSocket == INVALID_SOCKET) {
		WSACleanup();
		std::cout << "Could not initialize socket" << std::endl;
		std::cin.get();
		exit(0);
	}
	return LocalSocket;
}

SOCKET MySocket::initialize_udp_socket()
{
	SOCKET LocalSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	if (LocalSocket == INVALID_SOCKET)
	{
		WSACleanup(); 
		std::cout << "Could not initalize socket" << std::endl; 
		std::cin.get(); 
		exit(0);
	}
	return LocalSocket;
}

void MySocket::server_bind_socket() {
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET; //Address family type internet
	SvrAddr.sin_port = htons(this->Port); //port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address
	if ((bind(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(this->WelcomeSocket);
		WSACleanup();
		std::cout << "Could not bind to the socket" << std::endl;
		std::cin.get();
		exit(0);
	}
}

//puts the server socket in listening mode
void MySocket::server_listen_socket() {
	if (listen(this->WelcomeSocket, 1) == SOCKET_ERROR) {
		closesocket(this->WelcomeSocket);
		WSACleanup();
		std::cout << "Could not listen to the provided socket." << std::endl;
		std::cin.get();
		exit(0);
	}
	else {
		std::cout << "Waiting for client connection" << std::endl;
	}
}

// CLIENT

void MySocket::client_connect_to_tcp_server() {
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET; //Address family type internet
	SvrAddr.sin_port = htons(this->Port); //port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr(this->IPAddr.c_str()); //IP address
	if ((connect(this->WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(this->WelcomeSocket);
		WSACleanup();
		std::cout << "Could not connect to the server" << std::endl;
		std::cin.get();
		exit(0);
	}
}

// SEND and GET

void MySocket::SendData(const char* data, int numBytes)
{
	if (this->mySocket == SERVER)
	{
		send(this->ConnectionSocket, data, numBytes, 0);
	}
	else if (this->mySocket == CLIENT)
	{

		send(this->WelcomeSocket, data, numBytes, 0);
	}
}

int MySocket::GetData(char* data)
{
	int a = 0;
	if (this->mySocket == SERVER)
	{
		memset(data, 0, 100);
		a = recv(this->ConnectionSocket, this->Buffer, this->maxSize, 0);
		memcpy(data, this->Buffer, a);
	}
	else if (this->mySocket == CLIENT)
	{
		memset(data, 0, 100);
		a = recv(this->WelcomeSocket, this->Buffer, this->maxSize, 0);
		memcpy(data, this->Buffer, a);

	}

	return a;
}