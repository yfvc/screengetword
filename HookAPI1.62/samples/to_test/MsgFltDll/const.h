#ifndef _CONST_H_
#define _CONST_H_

typedef struct
{
	char *name;
	UINT id;
}MSG_INFO;

extern MSG_INFO g_msg_info[];
extern int g_msg_count;
#endif
