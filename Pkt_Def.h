#ifndef WINSOCK_H
#define WINSOCK_H

#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <string>
#define PKTSIZE_NO_BODY 7
enum ActionType{ FORWARD = 1, BACKWARD, RIGHT, LEFT, UP, DOWN, OPEN, CLOSE }; 
enum CmdType { DRIVE = 1, SLEEP, ARM, CLAW, ACK }; 

struct MotorBody
{
	unsigned char Direction;
	unsigned char Duration;
};
struct Header 
{
	unsigned int PktCount; //4 Bytes 0 1 2 3
	unsigned char Drive : 1; // bit begin 4 5 6 7
	unsigned char Status : 1;// 
	unsigned char Sleep : 1;// 
	unsigned char Arm : 1;
	unsigned char Claw : 1;
	unsigned char Ack : 1;
	unsigned char Padding : 2; // bit end 
	unsigned char Length : 8; //8
};
 
class PktDef /*: public winsock*/
{
private: 
	char* rawBuffer;
	char genBuffer[96];
	struct CmdPacket
	{
		Header h;
		char* Body; // 9 10
		unsigned char CRC; //11
	} theCmdPkt;
public: 
	PktDef();
	PktDef(char *);
	void EmptyPkt();
	void ClearPkt();
	void SetCmd(CmdType);
	void SetBodyData(char *, int);
	void SetPktCount(int);
	CmdType GetCmd();
	std::string GetCmdStr();
	bool getAck();
	bool PktDef::getStatus();
	int GetLength();
	char *GetBodyData();
	int GetPktCount();
	bool CheckCRC(char *, int);
	void CalcCRC();
	int GetBitsSet(char* pktBuffer);
	char * GenPacket();

	friend std::ostream& operator<<(std::ostream& os, PktDef&);
};

#endif WINSOCK_H
