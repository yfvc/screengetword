#ifndef _FILTER_H_

typedef struct
{
	UINT id;
	//char msg[40];
	char win_caption[256];
}FILTER_INFO;

class CMsgFilter
{
public:
	CMsgFilter();
	~CMsgFilter();

	FILTER_INFO *m_pFilterInfo;
	int m_nFilterInfoCount;
	int Filter(HWND, UINT, WPARAM, LPARAM);
	int FilterSendMessage(HWND hWnd, UINT msg, WPARAM, LPARAM);
	int FilterPostMessage(HWND hWnd, UINT msg, WPARAM, LPARAM);
	int FilterCallWindowProc(HWND hWnd, UINT msg, WPARAM, LPARAM);
	int FilterDispatchMessage(HWND hWnd, UINT msg, WPARAM, LPARAM);
};

#endif
