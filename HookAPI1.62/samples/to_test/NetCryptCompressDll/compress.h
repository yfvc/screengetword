#ifndef _COMPRESS_H_

typedef int (WINAPI *COMPRESSDATA)(char *, int, char *, int *);
typedef int (WINAPI *DECOMPRESSDATA)(char *, int, char *, int *);

typedef struct
{
	char app[128];
	char ip[16];
	char local_port_type[2];
	int server_port;
}COMPRESS_INFO;

class CompressFilter
{
public:
	CompressFilter();
	~CompressFilter();
	BOOL CompressCheck(char *remote_ip, int local_port, int remote_port);
	int CompressData(char *data, int data_len, char *data_compressed, int *pdata_compressed_len);
	int DecompressData(char *data, int data_len, char *data_decompressed, int *pdata_decompressed_len);

public:
	HANDLE hModCompress;
	COMPRESSDATA myCompressData;
	DECOMPRESSDATA myDecompressData;
	COMPRESS_INFO *m_pCompressInfo;
	int m_compress_info_count;
};
#endif
