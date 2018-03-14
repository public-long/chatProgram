#ifndef ZQH_NETWORK_TOOL_H__
#define ZQH_NETWORK_TOOL_H__

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

class NP_Socket_Lib
{
public:
	NP_Socket_Lib() { 
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,0), &wsaData);
	}
	~NP_Socket_Lib(){
		WSACleanup();
	}
};

extern NP_Socket_Lib g_theSocketLib;

SOCKET CreateConnectSocket(const char* pszIP, short int nPort);
SOCKET CreateServerSocket(const char* pszIP, short int nPort);
int Send_n(SOCKET s, const char* pBuff, int nLen);
int Recv_n(SOCKET s, char* pBuff, int nLen);
bool SendInt16(SOCKET s, short int n);
bool RecvInt16(SOCKET s, short int& n);
bool SendInt32(SOCKET s, int n);
bool RecvInt32(SOCKET s, int& n);

/*typedef struct Packet
{
	int nLen;
	char data[2048];
}Packet;

bool RecvPkt(SOCKET s, Packet* pkt);
bool SendPkt(SOCKET s, const char* pData, int nLen);*/

#endif //ZQH_NETWORK_TOOL_H__