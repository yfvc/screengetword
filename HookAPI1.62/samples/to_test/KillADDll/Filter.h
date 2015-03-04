#ifndef _FILTER_H_

#define FILTER_BIND		1
#define FILTER_CONNECT	2
#define FILTER_ACCEPT	3
#define FILTER_SEND		4
#define FILTER_RECV		5
#define FILTER_DNS		6
#define FILTER_SENDTO	FILTER_SEND
#define FILTER_RECVFROM	FILTER_RECV

#define FILTER_INFO_STEP 100
typedef struct
{
	char app[40];
	int type;
	char ip[16];
	int port;
	char buf[256];
}FILTER_INFO;

class CSocketFilter
{
public:
	CSocketFilter();
	~CSocketFilter();

	FILTER_INFO *m_pFilterInfo;
	HLOCAL m_hLocal;
	int m_nFilterInfoCount;
	int FilterConnect(char *ip, int port);
	int FilterBind(int port);
	int FilterRecv(int sd_connected, char *buf, int len);
	int FilterSend(int sd_connected, char *buf, int len);
	int FilterAccept(int sd_accepted);
	int FilterDNS(char *hostname);
	int FilterSendTo(char *ip, int port, char *buf, int len);
	int FilterRecvFrom(char *ip, int port, char *buf, int len);
};

#endif
