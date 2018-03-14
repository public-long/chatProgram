#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <winsock2.h>

#include "Protocol.h"
#include "Process.h"

typedef struct User
{
	char name[20];
	char password[20];
}User;

#define MAX_USER_NUM 63
User g_userDB[MAX_USER_NUM];
int  g_userNum = 0;

#define UNKNOWN_PKT_FORMAT	-1

#define PASSWORD_ERROR		-1
#define USER_UNFOUND		0
#define USER_FOUND			1

#define OPEN_USER_DB_FAIL	-1

typedef struct Session
{
	char userName[20];
	SOCKET sock;
	char buff[2048];
	int state;
}Session;

//int processPkt(const char* pkt, SOCKET sock, sockaddr* addr);
int processPkt(const char* pkt, Session* session);
//int processLoginPkt(const char* pkt, SOCKET sock, sockaddr* addr);
int processLoginPkt(const char* pkt, Session* session);
int processLogoutPkt(const char* pkt);

//int processPublicChatPkt(const char* pkt, SOCKET sock);
int processPublicChatPkt(const char* pkt);

//int broadcastPkt(PublicChatMsg* publicChatMsg, SOCKET sock);
int broadcastPkt(PublicChatMsg* publicChatMsg);

int checkUser(const User* userDB, int userNum, const User* user);
int loadUserDB(const char* fileName);

/*typedef struct Session
{
	char userName[20];
	sockaddr addr;
}Session;*/


#define STATE_UNLOGIN 0
#define STATE_LOGIN	  0

#define UNFOUND_SESSION 0
#define REMOVE_SESSION_SUCCESS 1

Session g_online_sessions[MAX_USER_NUM];
int     g_online_session_num = 0;

//int addSession(const char* userName, sockaddr* addr);
int addSession(const char* userName, SOCKET sNewConnection);
int removeSession(const char* userName);
//int removeSession(const sockaddr* addr);
int removeSession(const char* userName);

SOCKET createListenSocket(const char* ip, unsigned short port);

/*void workerThead(void* lpParam)
{
	SOCKET sNewConnection = (SOCKET)lpParam;
	while(1)
	{
		char buff[1024];
		int pktLen;
		bool bRet = RecvInt32(sNewConnection, pktLen);
		int recvSize = Recv_n(sNewConnection, 
			buff, pktLen-sizeof(int), 0);
		//������һ�����յ�һ�����������ݰ�
		if(recvSize != pktLen-sizeof(int))
			break;
		
		processPkt(buff, sNewConnection); 
	}
	//�ر���ͻ�ͨѶ���׽���
	closesocket(sNewConnection);
}*/

int main(int argc, char *argv[])
{
	//SOCKET sConnection[MAX_CONNECTION_NUM];
	int i;
	for(i=0; i<MAX_USER_NUM; i++)
	{
		g_online_sessions[i].sock = INVALID_SOCKET;
	}
	fd_set fd_read;

	if(OPEN_USER_DB_FAIL == loadUserDB("c:/users.txt"))
	{
		printf("open user file fail!\n");
		return -1;
	}

	//step1:��ʼ��Windows Socket 2.2��
	WSADATA              wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//step2:����һ���µ�Socket����Ӧ�ͻ��˵��������� 
	//SOCKET sListen = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKET sListen = createListenSocket(NULL, 8888);
	if(INVALID_SOCKET == sListen)
	{
		printf("���������׽���ʧ��\n");
		return -1;
	}

	printf("��ʼ����......\n");

	while(true)
	{
		FD_ZERO(&fd_read);
		FD_SET(sListen, &fd_read);

		for(i=1; i<g_online_session_num; i++)
		{
			FD_SET(g_online_sessions[i].sock, &fd_read);
		}

		int ret = select(0, &fd_read, NULL, NULL, NULL);
		if(SOCKET_ERROR == ret)
		{
			printf("select error\n");
			break;
		}

		if(ret > 0)
		{
			if(FD_ISSET(sListen,  &fd_read))
			{
				//�������׽��ּ��뵽sConnection��
				if(g_online_session_num == MAX_USER_NUM)
				{
					printf("connection is ...\n");
					
				}
				else
				{
					//��������
					g_online_sessions[g_online_session_num].sock = accept(sListen, NULL, NULL);
					if(INVALID_SOCKET == g_online_sessions[g_online_session_num].
						sock)
					{
						//������
						break;
					}
					g_online_sessions[g_online_session_num].state = STATE_UNLOGIN;
					g_online_session_num++;
				}				
			}
		}

		for(int i=1; i<g_online_session_num; i++)
		{
			
				if(FD_ISSET(g_online_sessions[i].sock, &fd_read))
				{
					//��������
					char buff[1024];
					int recvBytes = recv(g_online_sessions[i].sock, 
						buff, sizeof(buff), 0);
					if(recvBytes <= 0) //����ʱ�ر�
					{
						//sConnection[i] = INVALID_SOCKET;
						removeSession(g_online_sessions[i].userName);
					}

					processPkt(buff, &g_online_sessions[i]); 
					
					//��������
					//send(sConnection[i], message, recvBytes, 0);			
					
				}
			
		}
		
	}

	//step8:�ر��׽���
	closesocket(sListen);
	//step9���ͷ�Windows Socket DLL�������Դ
	WSACleanup();
	return 0;
}


//int processPkt(const char* pkt, SOCKET sock, sockaddr* addr)
int processPkt(const char* pkt, Session* session)
{
	int type = ((Header*)pkt)->type;
	//����ǵ�¼��������¼����
	if(session->state == STATE_UNLOGIN && LOGIN_PKT != type)
	{
		return UNKNOWN_PKT_FORMAT;
	}

	if(LOGIN_PKT == type)
	{
		//return processLoginPkt(pkt, sock, addr);
		return processLoginPkt(pkt, session);
	}
	else if(LOGOUT_PKT == type)//��������ǵǳ��������ǳ�����
	{
		//return processLogoutPkt(pkt);
		return processLogoutPkt(pkt);
	}
	else if(PUBLIC_CHAT_PKT == type)//��������ǹ��İ��������Ĵ���
	{
		//return processPublicChatPkt(pkt, sock);
		return processPublicChatPkt(pkt);
	}
	else //���򣬴�����
	{
		return UNKNOWN_PKT_FORMAT;
	}
	
}

//int processLoginPkt(const char* pkt, SOCKET sock, sockaddr* addr)
int processLoginPkt(const char* pkt, Session* session)
{
	User user;
	LoginPkt* loginPkt = (LoginPkt*)pkt;
	strcpy(user.name, loginPkt->userName);
	strcpy(user.password, loginPkt->password);
	if(USER_FOUND == checkUser(g_userDB, g_userNum, &user))
	{
		LoginRelpyPkt replyPkt;
		replyPkt.header.type = LOGIN_REPLY_PKT;
		replyPkt.retCode = LOGIN_SUCCESS;
		send(session->sock, (const char*)&replyPkt, sizeof(replyPkt), 
			0);

		PublicChatMsg publicChatMsg;
		publicChatMsg.header.type = PUBLIC_CHAT_PKT;
		strcpy(publicChatMsg.from, "system");
		sprintf(publicChatMsg.message, "%s��¼��", loginPkt->userName);
		broadcastPkt(&publicChatMsg);

		session->state = STATE_LOGIN;
		strcpy(session->userName, loginPkt->userName);

		//addSession(loginPkt->userName, sNewConnection);
	};

	return 0;
}

int processLogoutPkt(const char* pkt)
{
	LogoutPkt* logoutPkt = (LogoutPkt*)pkt;
	removeSession(logoutPkt->userName);

	return 0;
}

//int processPublicChatPkt(const char* pkt, SOCKET sock)
int processPublicChatPkt(const char* pkt)
{
	return broadcastPkt((PublicChatMsg*)pkt);
}

int checkUser(const User* userDB, int userNum, const User* user)
{
	for(int i=0; i<userNum; i++)
	{
		if(strcmp(userDB[i].name, user->name) == 0)
		{
			if(strcmp(userDB[i].password, user->password) == 0)
			{
				return USER_FOUND;
			}
			else 
			{
				return PASSWORD_ERROR;
			}
		}			
	}
	return USER_UNFOUND;
}

int loadUserDB(const char* fileName)
{
	FILE* fp = fopen(fileName, "r");
	if(NULL == fp)
		return OPEN_USER_DB_FAIL;

	while(!feof(fp))
	{
		fscanf(fp, "%s%s", g_userDB[g_userNum].name, g_userDB[g_userNum].password);
		g_userNum++;
	}

	return g_userNum;
}

/*int addSession(const char* userName, sockaddr* addr)
{
	strcpy(g_online_sessions[g_online_session_num].userName, userName);
	g_online_sessions[g_online_session_num].addr = *addr;
	g_online_session_num++;

	return 0;
}*/

int addSession(const char* userName, SOCKET sNewConnection)
{
	strcpy(g_online_sessions[g_online_session_num].userName, userName);
	g_online_sessions[g_online_session_num].sock = sNewConnection;
	g_online_session_num++;

	return 0;
}

int removeSession(const char* userName)
{
	for(int i=0; i<g_online_session_num; i++)
	{
		if(strcmp(g_online_sessions[i].userName, userName) == 0)
		{
			for(int j=i;  j<g_online_session_num-1; j++)
			{
				g_online_sessions[j] = g_online_sessions[j+1];
			}
			g_online_session_num--;
			return REMOVE_SESSION_SUCCESS;
		}
	}
	return UNFOUND_SESSION;
}

/*int removeSession(const sockaddr* addr)
{
	for(int i=0; i<g_online_session_num; i++)
	{
		if(g_online_sessions[i].addr == *addr)
		{
			for(int j=i;  j<g_online_session_num-1; j++)
			{
				g_online_sessions[j] = g_online_sessions[j+1];
			}
			g_online_session_num--;
			return REMOVE_SESSION_SUCCESS;
		}
	}
	return UNFOUND_SESSION;
}*/

/*int broadcastPkt(PublicChatMsg* publicChatMsg, SOCKET sock)
{
	for(int i=0; i<g_online_session_num; i++)
	{
		sendto(sock, (const char*)publicChatMsg, sizeof(PublicChatMsg), 0, 
			&(g_online_sessions[i].addr), sizeof(sockaddr));
		//���ҷ��������sendto��䣬�ҳ����п��ܴ��ڵ����⣻
		//������õؽ������
	}

	return 0;
}*/

int broadcastPkt(PublicChatMsg* publicChatMsg)
{
	for(int i=0; i<g_online_session_num; i++)
	{
		send(g_online_sessions[i].sock, 
			(const char*)publicChatMsg, 
			sizeof(PublicChatMsg), 0);
		//���ҷ��������sendto��䣬�ҳ����п��ܴ��ڵ����⣻
		//������õؽ������
	}

	return 0;
}


SOCKET createListenSocket(const char* ip, unsigned short port)
{
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == sListen)
		return INVALID_SOCKET;

	SOCKADDR_IN          local;
	local.sin_family = AF_INET;
	local.sin_port = htons(port);   
	if(ip == NULL)
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		local.sin_addr.s_addr = inet_addr(ip);
	//IP��ַΪINADDR_ANY��ʹ��htonl��IP��ַת��Ϊ�����ʽ

	//step4:�󶨼����˿�	  
	if(SOCKET_ERROR == bind(sListen, (SOCKADDR *)&local, sizeof(local)))
		return INVALID_SOCKET;

	//step5:��ʼ������ָ���������ֵȴ����г���Ϊ5
	//listen(sListen, 5); 
	listen(sListen, 5);

	return sListen;
}

