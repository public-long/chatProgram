#ifndef __ZQQ_PROTOCOL__H
#define __ZQQ_PROTOCOL__H

#define LOGIN_PKT			0
#define LOGOUT_PKT			1
#define LOGIN_REPLY_PKT		2
#define PUBLIC_CHAT_PKT		3
#define PRIVATE_CHAT_PKT	4

typedef struct Header
{
	//int len;
	int type;
}Header;

typedef struct LoginPkt
{
	Header header;
	char   userName[20];
	char   password[20];
}LoginPkt;

typedef struct LogoutPkt
{
	Header header;
	char   userName[20];
}LogoutPkt;

#define LOGIN_FAIL	  0
#define LOGIN_SUCCESS 1
typedef struct LoginRelpyPkt
{
	Header header;
	int		retCode;
}LoginRelpyPkt;

typedef struct PublicChatMsg
{
	Header header;
	char   from[20];
	char   message[512];
}PublicChatMsg;

typedef struct PrivateChatMsg
{
	Header header;
	char   from[20];
	char   to[20];
	char   message[512];
}PrivateChatMsg;

#endif //__ZQQ_PROTOCOL__H