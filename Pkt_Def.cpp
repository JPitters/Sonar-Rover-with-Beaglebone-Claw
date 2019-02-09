#include "Pkt_Def.h" 

void PktDef::SetCmd(CmdType type)
{
	char * buffer = new char[sizeof(CmdPacket)];
	if (type == DRIVE)
	{
		ClearPkt();
		theCmdPkt.h.Drive = 1;// theCmdPkt.
	}
	else if (type == SLEEP)
	{
		ClearPkt();
		theCmdPkt.h.Sleep = 1;
	}
	else if (type == ARM)
	{
		ClearPkt();
		theCmdPkt.h.Arm = 1;
	}
	else if (type == CLAW)
	{
		ClearPkt();
		theCmdPkt.h.Claw = 1;
	}
	else if (type == ACK)
	{
		ClearPkt();
		theCmdPkt.h.Ack = 1;
	}

}
PktDef::PktDef()
{
	EmptyPkt();
	ClearPkt();	
}
PktDef::PktDef(char *src)
{
	EmptyPkt();
	ClearPkt();

	memcpy((char*)&theCmdPkt, src, PKTSIZE_NO_BODY-1);
	theCmdPkt.Body = new char[(int)theCmdPkt.h.Length - PKTSIZE_NO_BODY];
	memcpy(theCmdPkt.Body, &src[PKTSIZE_NO_BODY-1], theCmdPkt.h.Length - PKTSIZE_NO_BODY);
	memcpy(&theCmdPkt.CRC, &src[theCmdPkt.h.Length-1], 1);

}
void PktDef::EmptyPkt()
{
	theCmdPkt.h.PktCount = 0;
	theCmdPkt.h.Length = PKTSIZE_NO_BODY;
	delete[] theCmdPkt.Body;
	theCmdPkt.Body = nullptr;
	theCmdPkt.CRC = 0;
}
void PktDef::ClearPkt()
{
	rawBuffer = nullptr;
	theCmdPkt.h.Drive = 0;
	theCmdPkt.h.Status = 0;
	theCmdPkt.h.Sleep = 0;
	theCmdPkt.h.Arm = 0;
	theCmdPkt.h.Claw = 0;
	theCmdPkt.h.Ack = 0;
	theCmdPkt.h.Padding = 0;
}
void PktDef::SetBodyData(char * mBody, int size)
{
	theCmdPkt.Body = new char[size];
	memcpy(theCmdPkt.Body, mBody, size);
	theCmdPkt.h.Length = PKTSIZE_NO_BODY + size;
	//rawBuffer = GenPacket();

}
void PktDef::SetPktCount(int pktc)
{
	theCmdPkt.h.PktCount = pktc;
}

CmdType PktDef::GetCmd()
{
		if (theCmdPkt.h.Drive) {
			return CmdType(1);
		}
		else if (theCmdPkt.h.Status)
		{
			return CmdType(2);
		}
		else if (theCmdPkt.h.Sleep)
		{
			return CmdType(3);
		}
		else if(theCmdPkt.h.Claw)
		{
			return CmdType(4);
		}
		return CmdType();
}

std::string PktDef::GetCmdStr()
{
	if (theCmdPkt.h.Drive) {
		return "Drive";
	}
	else if (theCmdPkt.h.Status)
	{
		return "Status";
	}
	else if (theCmdPkt.h.Sleep)
	{
		return "Sleep";
	}
	else if (theCmdPkt.h.Claw)
	{
		return "Claw";
	}
	return "None";
}

bool PktDef::getAck()
{
	if (theCmdPkt.h.Ack)
		return true;
	return false;
}
bool PktDef::getStatus()
{
	if (theCmdPkt.h.Status)
		return true;
	return false;
}
int PktDef::GetLength()
{
	return theCmdPkt.h.Length;
}
char * PktDef::GetBodyData()
{
	return reinterpret_cast<char*>(&theCmdPkt.Body);
}
int PktDef::GetPktCount()
{
	return theCmdPkt.h.PktCount;
}
bool PktDef::CheckCRC(char * buffer, int size)
{
	if ((int)theCmdPkt.CRC == (int)buffer[size-1])
		return true;
	return false;
}
void PktDef::CalcCRC()
{
	theCmdPkt.CRC = 0;
	int counter = 0;
	char* ptr = new char[theCmdPkt.h.Length];
	GenPacket();
	memcpy(ptr, rawBuffer, theCmdPkt.h.Length);
	
	//Nine because of extra three byted added on CRC
	for (size_t j = 0; j < theCmdPkt.h.Length; j++)
	{
		for (size_t i = 0; i < 8; i++)
		{
			counter += *ptr & 1;
			*ptr >>= 1;
		}

		ptr += 1;
	}
	theCmdPkt.CRC = counter;
}
int PktDef::GetBitsSet(char* pktBuffer)
{
	int counter = 0;
	char temp[12];
	memcpy(temp, pktBuffer, 12);
	char* ptr = temp;

	//Nine because of extra three byted added on CRC
	for (size_t j = 0; j < 11; j++)
	{
		for (size_t i = 0; i < 8; i++)
		{
			counter += *ptr & 1;
			*ptr >>= 1;
		}

		ptr += 1;
	}
	return counter;
}

char* PktDef::GenPacket()
{
	if (rawBuffer != nullptr) 
	{
		//delete[] rawBuffer;
		rawBuffer = nullptr;
	}
		rawBuffer = new char[theCmdPkt.h.Length];
		memset(rawBuffer, 0, theCmdPkt.h.Length);
		memcpy(rawBuffer, &theCmdPkt.h, PKTSIZE_NO_BODY - 1); //Include bitfield
		memcpy(&rawBuffer[PKTSIZE_NO_BODY - 1], theCmdPkt.Body, theCmdPkt.h.Length - PKTSIZE_NO_BODY);
		memcpy(&rawBuffer[theCmdPkt.h.Length - 1], &theCmdPkt.CRC, 1);

	
	return rawBuffer;
}

std::ostream& operator<<(std::ostream& os , PktDef& src) 
{
	bool valid = true;

	char *ptr;
	ptr = src.GenPacket();

	char locBuffer[12];
	memcpy(locBuffer, ptr, 12);

	int calcdCRC = src.GetBitsSet(locBuffer);
	bool statusSet = (bool)((locBuffer[4] >> 1) & 0x01); // status is 7th bit due to reverse endianness

	if ((calcdCRC != (int)locBuffer[11]) || !statusSet)
	{
		os << "Bad Packet. Dropping Processing." << std::endl;
	}
	else
	{
		int driveSet = (int)(locBuffer[10] & 0x01);
		int armUpSet = (int)((locBuffer[10] >> 1) & 0x01);
		int armDownSet = (int)((locBuffer[10] >> 2) & 0x01);
		int clawOpenSet = (int)((locBuffer[10] >> 3) & 0x01);
		int clawClosedSet = (int)((locBuffer[10] >> 4) & 0x01);

		os << "Raw Data:";
		for (int x = 0; x < (int)src.theCmdPkt.h.Length; x++)
			os << std::hex << std::setw(2) << (unsigned int)*(ptr++) << ", ";
		os << std::endl << std::dec << "CommandID: " << (int)src.GetCmd() << ", CommandType: " << src.GetCmdStr() << std::endl;
		os << calcdCRC << ", " << statusSet << std::endl;
		os << "Sonar: " << int((unsigned char)locBuffer[7] << 8 | (unsigned char)locBuffer[6]) << ", Arm: " << int((unsigned char)locBuffer[9] << 8 | (unsigned char)locBuffer[8]) << std::endl;

		os << std::dec << "Drive flag: " << driveSet << std::endl;
		if (armUpSet && clawOpenSet)
		{
			os << "Arm is Up, Claw is Open" << std::endl;
		}
		if (armDownSet && clawOpenSet)
		{
			os << "Arm is Down, Claw is Open" << std::endl;
		}
		if (armUpSet && clawClosedSet)
		{
			os << "Arm is Up, Claw is Closed" << std::endl;
		}
		if (armDownSet && clawClosedSet)
		{
			os << "Arm is Down, Claw is Closed" << std::endl;
		}
	}
	
	return os;
}
