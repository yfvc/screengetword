#pragma once

typedef struct tag_KEY_INFO
{
	int sd;
	unsigned char key[17];
	int key_len;
	tag_KEY_INFO *pnext;
}KEY_INFO;

class CKeyManager
{
public:
	CKeyManager(void);
	~CKeyManager(void);

	CRITICAL_SECTION m_cs_key;
	KEY_INFO *m_pkey_info;
	int m_key_info_count;

	int AddKey(int sd, unsigned char *key, int key_len);
	int RemoveKey(int sd);
	void RemoveAllKey();
	int GenKey(unsigned char * key);
	KEY_INFO *GetKey(int sd);
};
