#include "./MySocket.h"
#include "./Pkt_Def.h"

bool ExeComplete = false; 

void cmdThread()
{
	char ipaddr[128], chport[128], chmaxbuff[128], command[28], direction[28], chDur[300];;
	int port, maxbuffsize, pktcount = 0;;
	char *ptr;

	std::cout << "Enter ipaddr: " << std::endl;
	std::cin.getline(ipaddr, sizeof(ipaddr));
	std::cout << "Enter port: " << std::endl;
	std::cin.getline(chport, sizeof(chport));
	port = atoi(chport);
	std::cout << "Enter max buffer size: " << std::endl;
	std::cin.getline(chmaxbuff, sizeof(chmaxbuff));
	maxbuffsize = atoi(chmaxbuff);
	MySocket CmdClient(SocketType::CLIENT, ipaddr, port, ConnectionType::TCP, maxbuffsize); //creating mysocket
	CmdClient.ConnectTCP();  //3-way handshake

	MotorBody generalCmd;
	PktDef txPkt, rxPkt; 

	while (!ExeComplete)
	{
		std::cout << "Creating packet now.." << std::endl;
		std::cout << "Enter the command type(d,s,a,c)" << std::endl; //drive,sleep,arm,claw
		std::cin.getline(command, sizeof(command));
		if (strcmp(command, "d") == 0)
		{
		  //---DRIVE BIT SET TO 1 IN HEADER
			txPkt.SetCmd(DRIVE);
		  
		  //-------------------MOTOR BODY--------------------
			std::cout << "Setting up a motor body.." << std::endl;
		  //----------------SETS UP DIRECTION----------------
			std::cout << "Please enter the direction(f,b,r,l): " << std::endl; 
			std::cin.getline(direction, sizeof(direction));
			if (strcmp(direction, "f") == 0) { generalCmd.Direction = FORWARD; }
			else if (strcmp(direction, "b") == 0) { generalCmd.Direction = BACKWARD; }
			else if (strcmp(direction, "r") == 0) { generalCmd.Direction = RIGHT; }
			else if (strcmp(direction, "l") == 0) { generalCmd.Direction = LEFT; }
		  //----------------SETS UP DURATION----------------
			std::cout << "Please enter the duration: " << std::endl; //Asks user for 
			std::cin.getline(chDur, sizeof(chDur));
			generalCmd.Duration = atoi(chDur);
			txPkt.SetBodyData((char *)&generalCmd, sizeof(MotorBody)); //sets the body 
			
			pktcount++; 
			txPkt.SetPktCount(pktcount); 
			txPkt.CalcCRC();
			ptr = txPkt.GenPacket(); 
			CmdClient.SendData(ptr, txPkt.GetLength()); 
		}
		else if (strcmp(command, "s") == 0)
		{
			txPkt.SetCmd(SLEEP);
			ExeComplete = true;
			generalCmd.Duration = 0;

			pktcount++;
			txPkt.SetPktCount(pktcount);
			txPkt.CalcCRC();
			ptr = txPkt.GenPacket();
			CmdClient.SendData(ptr, txPkt.GetLength());
		}
		else if (strcmp(command, "a") == 0)
		{
			txPkt.SetCmd(ARM); //Sets the header 

			std::cout << "Setting up a motor body.." << std::endl;
			std::cout << "Please enter the direction(u,d): " << std::endl; //forward,backwords, right, left
			std::cin.getline(direction, sizeof(direction));
			if (strcmp(direction, "u") == 0) { generalCmd.Direction = UP; }
			else if (strcmp(direction, "d") == 0) { generalCmd.Direction = DOWN; }
			generalCmd.Duration = 0;
			txPkt.SetBodyData((char *)&generalCmd, 2); //sets the body 

			pktcount++;
			txPkt.SetPktCount(pktcount);
			txPkt.CalcCRC();
			ptr = txPkt.GenPacket();
			CmdClient.SendData(ptr, txPkt.GetLength());
		}
		else if (strcmp(command, "c") == 0)
		{
			txPkt.SetCmd(CLAW); //Sets the header 

			std::cout << "Setting up a motor body.." << std::endl;

			std::cout << "Please enter the option(o,c): " << std::endl; //forward,backwords, right, left
			std::cin.getline(direction, sizeof(direction));
			if (strcmp(direction, "o") == 0) { generalCmd.Direction = OPEN; }
			else if (strcmp(direction, "c") == 0) { generalCmd.Direction = CLOSE; }
			generalCmd.Duration = 0;
			txPkt.SetBodyData((char *)&generalCmd, 2); //sets the body 

			pktcount++;
			txPkt.SetPktCount(pktcount);
			txPkt.CalcCRC();
			ptr = txPkt.GenPacket();
			CmdClient.SendData(ptr, txPkt.GetLength());
		}
		char* rxBuffer = new char[maxbuffsize];
		memset(rxBuffer, 0, maxbuffsize);
		int a = CmdClient.GetData(rxBuffer);
		PktDef recvB(rxBuffer);
		//delete[] & generalCmd;
		

	}
	CmdClient.DisconnectTCP();
}

void teleThread()
{
	char ipaddr[128], chport[128], chmaxbuff[128], buffer[128], direction[28], chDur[300];
	int port, maxbuffsize, motorbodyDuration, pktcount = 0;
	char *ptr;

	std::cout << "Enter ipaddr: " << std::endl;
	std::cin.getline(ipaddr, sizeof(ipaddr));
	std::cout << "Enter port(27501): " << std::endl;
	std::cin.getline(chport, sizeof(chport));
	port = atoi(chport);
	std::cout << "Enter max buffer size: " << std::endl;
	std::cin.getline(chmaxbuff, sizeof(chmaxbuff));
	maxbuffsize = atoi(chmaxbuff);
	MySocket teleClient(SocketType::CLIENT, ipaddr, port, ConnectionType::TCP, maxbuffsize); //creating mysocket
	teleClient.ConnectTCP();  //3-way handshake 

	while (!ExeComplete) 
	{
		int a = teleClient.GetData(buffer);
		PktDef rxPkt(buffer);
		std::cout << rxPkt << std::endl;
	}
	teleClient.DisconnectTCP();

}
int main()
{
	std::cout << "What thread do you want to open?(c,t)" << std::endl;
	char answer;
	answer = std::cin.get();
	std::cin.ignore();
	if (answer == 'c')
		std::thread(&cmdThread).detach();
	if (answer == 't')
		std::thread(&teleThread).detach();

	while (true) {}
	return 0;
}
