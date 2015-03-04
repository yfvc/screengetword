#ifndef _FILTER_H_

typedef struct
{
	char app[40];
}FILTER_INFO;

class CFilter
{
public:
	CFilter();
	~CFilter();

	FILTER_INFO *m_pFilterInfo;
	int m_nFilterInfoCount;
	int FilterName(char *name);
};

#endif
