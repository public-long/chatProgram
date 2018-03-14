#include <stdio.h>
#include "NetworkTool.h"

NP_Socket_Lib g_theSocketLib;

SOCKET CreateConnectSocket(const char* pszIP, short int nPort)
{
	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == client)
	{
		return INVALID_SOCKET;
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = inet_addr(pszIP); 
	serverAddr.sin_port = htons(nPort); 

	if(0 != connect(client, (SOCKADDR*)&serverAddr, sizeof(serverAddr)))
	{
		closesocket(client);
		return INVALID_SOCKET;
	}
	
	return client;
}

SOCKET CreateServerSocket(const char* pszIP, short int nPort)
{
	SOCKET server = socket(AF_INET,SOCK_STREAM,0);
	if(server == INVALID_SOCKET)
    {
        //printf("socket() error: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
    }
	
	//创建本地实体
    sockaddr_in local;
	memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET; 
//	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_addr.s_addr = (
		pszIP != NULL ? inet_addr(pszIP) : INADDR_ANY); 
    local.sin_port = htons(nPort); 

	//将服务器本地套接字和本地实体绑定
    if(bind(server, (struct sockaddr *)&local, sizeof(local))!=0)
    {
		//printf("bind() error: %d\n", WSAGetLastError());
		closesocket(server);
		return INVALID_SOCKET;
    }
	
	//listen
	listen(server, 5);
	return server;
}

int Send_n(SOCKET s, const char* pBuff, int nLen)
{
	int nTotalSent = 0;
	while(nTotalSent != nLen)
	{
		int nSent = send(s, pBuff+nTotalSent, nLen-nTotalSent, 0);
		if(nSent < 0)
		{
			return nTotalSent;
		}
		else if(nSent == 0)
		{
			Sleep(100);
		}
		nTotalSent += nSent;
	}
	return nTotalSent;
}

int Recv_n(SOCKET s, char* pBuff, int nLen)
{
	int nTotalRecv = 0;
	while(nTotalRecv != nLen)
	{
		int nRecv = recv(s, pBuff+nTotalRecv, nLen-nTotalRecv, 0);
		if(nRecv <= 0)
		{
			return nTotalRecv;
		}
		nTotalRecv += nRecv;
	}
	return nTotalRecv;
}

bool SendInt16(SOCKET s, short int n)
{
	short int t = htons(n);
	return sizeof(t) == Send_n(s, (char*)&t, sizeof(t));
}

bool RecvInt16(SOCKET s, short int& n)
{
	short int t;
	if(sizeof(t) == Recv_n(s, (char*)&t, sizeof(t)))
	{
		n = ntohs(t);
		return true;
	}
	return false;
}

bool SendInt32(SOCKET s, int n)
{
	int t = htonl(n);
	return sizeof(t) == Send_n(s, (char*)&t, sizeof(t));
}

bool RecvInt32(SOCKET s, int& n)
{
	int t;
	if(sizeof(t) == Recv_n(s, (char*)&t, sizeof(t)))
	{
		n = ntohl(t);
		return true;
	}
	return false;
}

/*bool RecvPkt(SOCKET s, Packet* pkt)
{
	int nLen;
	if(!RecvInt32(s, nLen))
	{
		return false;
	}

	pkt->nLen = nLen;
	if(nLen != Recv_n(s, pkt->data, nLen))
	{
		return false;
	}
	
	return true;
}

bool SendPkt(SOCKET s, const char* pData, int nLen)
{
	if(!SendInt32(s, nLen))
	{
		return false;
	}

	if(nLen != Send_n(s, pData, nLen))
		return false;

	return true;
}*/