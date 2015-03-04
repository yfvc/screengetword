#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

//#include <winsock2.h>

#include "setenv.h"
#include "ca_common.h"
#include "ca_error.h"

#include "rsaref.h"
#include "csp_api.h"
#include "csp_self.h" 

#define FIRST	1
#define MID		2
#define LAST	3
#define ONLY	4
#define SUCCESS 0

static unsigned short static_inbuf[2560];  

void gen_i_enkey(unsigned short *inkey, unsigned short *enkey_array);
void gen_i_dekey(unsigned short *enkey_array, unsigned short *dekey_array);
void cip(unsigned short *indata, unsigned short *outdata, unsigned short *key);
int  ecb_encry_proc(unsigned short *indata,unsigned short indata_blk,
                    unsigned short *outdata,unsigned short *key);
int  ecb_decry_proc(unsigned short *indata,unsigned short indata_blk,
                    unsigned short *outdata,unsigned short *key);
int  ecb_mac_proc(unsigned short *indata1,unsigned short indata1_blk,
                  unsigned short *outMAC,unsigned short *key);

#define  maxim   65537
unsigned short g_tempenkey[60],g_tempdekey[60];

R_RANDOM_STRUCT  gRandomStruct;
void gen_i_enkey(unsigned short *inkey, unsigned short *enkey_array)

{
    short i,k;
    unsigned short *p;
        k = 0;
        for(i=0;i<8;i++){
           enkey_array[k++] = inkey[i];
        }

        p = enkey_array;
        for(i=0;i<6;i++){
            enkey_array[k++] = (p[1] << 9) | (p[2] >> 7); /*  0 */
            enkey_array[k++] = (p[2] << 9) | (p[3] >> 7); /*  1 */
            enkey_array[k++] = (p[3] << 9) | (p[4] >> 7); /*  2 */
            enkey_array[k++] = (p[4] << 9) | (p[5] >> 7); /*  3 */
            enkey_array[k++] = (p[5] << 9) | (p[6] >> 7); /*  4 */
            enkey_array[k++] = (p[6] << 9) | (p[7] >> 7); /*  5 */
            enkey_array[k++] = (p[7] << 9) | (p[0] >> 7); /*  6 */
            enkey_array[k++] = (p[0] << 9) | (p[1] >> 7); /*  7 */
            p = p + 8;
        }
        return;
}

unsigned short inv(unsigned short xin)
{
    long n1,n2,q,r,b1,b2,t;
        if(xin == 0){
            b2 = 0;
        }
        else{
            n1 = maxim;
            n2 = xin;
            b2 = 1;
            b1 = 0;
            do{
                r = (n1 % n2);
                q = n1/n2;
                if(r == 0){
                    if(b2 < 0){
                        b2 = maxim + b2;
                    }
                }
                else{
                    n1 = n2;
                    n2 = r;
                    t = b2;
                    b2 = b1 - q*b2;
                    b1 = t;
                }
            }while(r != 0);
        }
        return (unsigned short)b2;

}

void gen_i_dekey(unsigned short *enkey_array, unsigned short *dekey_array)
{
    short i,k;
    unsigned short *p;
        p = enkey_array + 46;
        k = 0;
        dekey_array[k++] = inv(p[2]);
        dekey_array[k++] = -p[3];
        dekey_array[k++] = -p[4];
        dekey_array[k++] = inv(p[5]);
        dekey_array[k++] = p[0];
        dekey_array[k++] = p[1];
        p = p - 6;
        for(i=0;i<7;i++){
            dekey_array[k++] = inv(p[2]);
            dekey_array[k++] = -p[4];
            dekey_array[k++] = -p[3];
            dekey_array[k++] = inv(p[5]);
            dekey_array[k++] = p[0];
            dekey_array[k++] = p[1];
            if(i != 6){
                p = p - 6;
            }
        }

        dekey_array[k++] = inv(enkey_array[0]);
        dekey_array[k++] = -enkey_array[1];
        dekey_array[k++] = -enkey_array[2];
        dekey_array[k++] = inv(enkey_array[3]);
        return;
}


unsigned short mul(unsigned short a, unsigned short b)
{
    unsigned long lx;
    unsigned short low,high,x;
        if(a){
            if(b){
                lx = a;
                lx = lx*b;
		low = (unsigned short)lx;
		high = (unsigned short)(lx >>16);
                x = low - high;
                if(low < high){
                    x++;
                }
                return (unsigned short)x;
            }
            else{
                return (unsigned short)(1-a);
            }
        }
        else{
            return (unsigned short)(1-b);
        }
}

void cip(unsigned short *indata, unsigned short *outdata, unsigned short *key)
{

	ideaCrypt( indata, outdata, key );
 
/*	
	unsigned short x1,x2,x3,x4,t1,t2;
    unsigned short *p,i;

        p = key;
        x1 = indata[0];
        x2 = indata[1];
        x3 = indata[2];
        x4 = indata[3];
        for(i=0;i<8;i++){
            x1 = mul(x1,p[0]);
            x2 = x2 + p[1];
            x3 = x3 + p[2];
            x4 = mul(x4,p[3]);
            t2 = mul(x1 ^ x3,p[4]);
            t1 = t2 + (x2^x4);
            t1 = mul(t1,p[5]);
            t2 = t1 + t2;
            x1 = x1 ^ t1;
            x4 = x4 ^ t2;

            t2 = t2 ^ x2;
            x2 = x3 ^ t1;
            x3 = t2;

            p = p + 6;
        }

        outdata[0] = mul(x1,p[0]);
        outdata[1] = x3 + p[1];
        outdata[2] = x2 + p[2];
        outdata[3] = mul(x4,p[3]);
*/
        return;
}

int  ecb_encry_proc(unsigned short *indata,unsigned short indata_blk,
                    unsigned short *outdata,unsigned short *key)
{
    unsigned short i;
    unsigned short *inp, *outp;
	WORD eKey[ 52 ], dKey[ 52 ];
  	ideaExpandKey( key, eKey, dKey );

//       gen_i_enkey(key, g_tempenkey);
        inp = indata;
        outp = outdata;
        for(i=0;i<indata_blk;i++){

            cip(inp, outp, eKey);
			//outp[0]=htons(outp[0]);
			//outp[1]=htons(outp[1]);
			//outp[2]=htons(outp[2]);
			//outp[3]=htons(outp[3]);

            inp = inp + 4;
            outp = outp + 4;
        }
        return 0;

}


int  ecb_decry_proc(unsigned short *indata,unsigned short indata_blk,
                    unsigned short *outdata,unsigned short *key)
{
    unsigned short i;
    unsigned short *inp, *outp;
	WORD eKey[ 52 ], dKey[ 52 ];
  	ideaExpandKey( key, eKey, dKey );

//        gen_i_enkey(key, g_tempenkey);
//        gen_i_dekey(g_tempenkey, g_tempdekey);
        inp = indata;
        outp = outdata;
        for(i=0;i<indata_blk;i++){
			//inp[0]=ntohs(inp[0]);
			//inp[1]=ntohs(inp[1]);
			//inp[2]=ntohs(inp[2]);
			//inp[3]=ntohs(inp[3]);
            cip(inp, outp, dKey);
            inp = inp + 4;
            outp = outp + 4;
        }
        return 0;
}

int  ecb_mac_proc(unsigned short *indata1,unsigned short indata1_blk,
                  unsigned short *outMAC,unsigned short *key)
{
    unsigned short i,j,k;
    unsigned short tbuf[4];
    unsigned short *inp;
        gen_i_enkey(key, g_tempenkey);

        inp = indata1;
        tbuf[0] = outMAC[0];
        tbuf[1] = outMAC[1];
        tbuf[2] = outMAC[2];
        tbuf[3] = outMAC[3];

        j = indata1_blk;
        for(i=0;i<j;i++){
            for(k=0;k<4;k++){
                tbuf[k] ^= inp[k];
            }
            cip(tbuf, tbuf, g_tempenkey);
            inp = inp + 4;
        }

        for(i=0;i<4;i++){
            outMAC[i] = tbuf[i];
        }

        return 0;
}

BYTE generate_soft_mac_core(WORD c_status,BYTE *msg_text,WORD msg_size,
				BYTE *hash_text,WORD *hash_size,WORD *save_buf);
BYTE enc_message_sdbi(WORD c_status,BYTE *plain_text,
				WORD plain_size, BYTE *cipher_text, WORD *cipher_size,WORD *save_buf);
BYTE dec_message_sdbi(WORD c_status,BYTE *plain_text,WORD *plain_size,
			BYTE *cipher_text, WORD cipher_size,WORD *save_buf);

/*------------------ API  functions prototype --------------*/
WORD    Enc_Message_SDBI(WORD  status,BYTE *plain_text,
		WORD plain_size,BYTE *cipher_text,WORD *cipher_size,
		BYTE *save_buf);
WORD    Dec_Message_SDBI(WORD  status,BYTE *plain_text,
		WORD *plain_size,BYTE *cipher_text,WORD cipher_size,
		BYTE *save_buf);
WORD    Gen_MAC(WORD  status,BYTE *msg_text,WORD msg_size,
		BYTE *mac_text,BYTE *save_buf);
 
 
WORD Enc_Message_SDBI(
	WORD  status,
	BYTE *plain_text,
	WORD plain_size,
	BYTE *cipher_text,
	WORD *cipher_size,
	BYTE *key)
{
 
	BYTE    func_serial, ret_val;
 	int     cipher_len = 0;
	WORD    kdormk = 1;
	 
	func_serial = E_M_S;
	if((status == MID) || (status == FIRST)) {
		if(plain_size != 2*MAX_TEXT_LEN) {
			return 0xc3;
		}
	}
	if((status == LAST) || (status == ONLY)) {
		if(plain_size > 2*MAX_TEXT_LEN) {
			return 0xc4;
		
		}
	}

	 
	if((ret_val = enc_message_sdbi(status,plain_text,
			plain_size,cipher_text,cipher_size,(WORD*)key)) !=0) {
		return(ret_val);
	}
	

	return(SUCCESS);
}


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*  Dec_Message_SDBI
*  return : 0 --- success, else --- failure
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
WORD Dec_Message_SDBI(
	WORD  status,
	BYTE *plain_text,
	WORD *plain_size,
	BYTE *cipher_text,
	WORD cipher_size,
	BYTE *key)
{
	 
	BYTE    func_serial, ret_val;
 	int     plain_len = 0;
 
	
	func_serial = Dec_M_S;

	if((cipher_size % 8) != 0) {
		return 0xc9;
	}

	if((status == MID) || (status == FIRST)) {
		if(cipher_size != 2*MAX_TEXT_LEN) {
				return 0xc9;
		}
}
	if((status == LAST) || (status == ONLY)) {
		if(cipher_size > 2*MAX_TEXT_LEN) {
			return 0xc4;
		}
	}
	if((ret_val = dec_message_sdbi(status,plain_text,
			plain_size,cipher_text,cipher_size,(WORD *)key)) !=0) {
			return ret_val;
	}
	return(SUCCESS);
}



/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*  Gen_MAC
*  return : 0 --- success, else --- failure
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
WORD Gen_MAC(
	WORD  status,
	BYTE *msg_text,
	WORD msg_size,
	BYTE *mac_text,
	BYTE *key)   /* 16 bytes */
{
	 
	BYTE    func_serial, ret_val;
	WORD     o_len;
	 
	 
	WORD    hashormac = 2;
	 

	func_serial = Gen_M;

	if((status == MID) || (status == FIRST)) {
		if(msg_size != 2*MAX_TEXT_LEN) {
			return 0xc8;
		}
	}
	if((status == LAST) || (status == ONLY)) {
		if(msg_size > 2*MAX_TEXT_LEN) {
			return 0xc4;
		}
	}

	if((ret_val = generate_soft_mac_core(status,msg_text,
			msg_size,mac_text,&o_len,(WORD *)key)) !=0) {
	return ret_val;
	}
	return(SUCCESS);
}
 
BYTE enc_message_sdbi(WORD c_status,
	BYTE *plain_text,
	WORD plain_size,
	BYTE *cipher_text,
	WORD *cipher_size,
	WORD *save_buf)
{
	WORD    *inbuf;
	int     i;
	WORD    *word_point, cfb_block=8;
	int pad_bytes,pad_zero_no;
	int first_sndword,tail_bytes,indata_len;
	BYTE  last8bytes[10];
	WORD kd_buf[64];

	inbuf=static_inbuf;
 	memcpy(kd_buf,save_buf,64);
	switch(c_status) {
	    case 1:
		if(plain_size!=(MAX_TEXT_LEN+MAX_TEXT_LEN)){
			return(0xc3);
		}
		ecb_encry_proc((unsigned short *)plain_text,MAX_TEXT_LEN/4,
                    (WORD *)cipher_text,kd_buf);

		*cipher_size=MAX_TEXT_LEN*2;
		break;

	    case 2:
		if(plain_size != (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc3);
		}

		ecb_encry_proc((unsigned short *)plain_text,MAX_TEXT_LEN/4,
                    (WORD*)cipher_text,kd_buf);
		*cipher_size=MAX_TEXT_LEN*2;
		break;
	    case 3:
		if(plain_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}
		cfb_block = 8;

		pad_bytes = cfb_block - (plain_size % cfb_block);
		pad_zero_no = pad_bytes - 1;

		tail_bytes = plain_size;
		first_sndword = tail_bytes >> 1;

		inbuf[0]=1+((tail_bytes+pad_bytes) >>1);
		inbuf[1]=ENC_MSG_LAST_ECB;
		word_point = (WORD *)plain_text;
		for(i=0;i<first_sndword;i++)
			inbuf[i+2] = word_point[i];

		if((tail_bytes % 2) ==1) {
			last8bytes[0]=plain_text[plain_size - 1];
			last8bytes[1]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+2]=0;
		}
		else {
			last8bytes[0]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+1]=0;
		}

		indata_len=(pad_bytes+1) >> 1;
		word_point = (WORD *)last8bytes;
		for(i=0;i<indata_len;i++)
			inbuf[i+2+first_sndword] = word_point[i];
		ecb_encry_proc((unsigned short *)&inbuf[2],(WORD)((inbuf[0]-1)/4),
                    (WORD*)cipher_text,(WORD*)kd_buf);
		*cipher_size=(inbuf[0]-1)*2;
		break;

	    case 4:
		if(plain_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}

		cfb_block = 8;

		pad_bytes = cfb_block - (plain_size % cfb_block);
		pad_zero_no = pad_bytes - 1;

		tail_bytes = plain_size;
		first_sndword = tail_bytes >> 1;

		inbuf[0]=1+((tail_bytes+pad_bytes) >>1);
		word_point = (WORD *)plain_text;
		for(i=0;i<first_sndword;i++)
			inbuf[i+2] = word_point[i];

		if((tail_bytes % 2) ==1) {
			last8bytes[0]=plain_text[plain_size - 1];
			last8bytes[1]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+2]=0;
		}
		else {
			last8bytes[0]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+1]=0;
		}

		indata_len=(pad_bytes+1) >> 1;
		word_point = (WORD *)last8bytes;
		for(i=0;i<indata_len;i++)
			inbuf[i+2+first_sndword] = word_point[i];

		ecb_encry_proc((unsigned short *)&inbuf[2],(WORD)((inbuf[0]-1)/4),
                    (WORD*)cipher_text,(WORD*)kd_buf);
		*cipher_size=(inbuf[0]-1)*2;
		break;
	    default:
		return(0xff);
	}
	return(SUCCESS);
}


 
BYTE dec_message_sdbi(WORD c_status,BYTE *plain_text,WORD *plain_size,
			BYTE *cipher_text,WORD cipher_size,WORD *save_buf)
{
	WORD    *inbuf;
 	int     i;
 	unsigned char *p;
	int tail_posi;
 	WORD kd_buf[64];

	inbuf=static_inbuf;
 	memcpy(kd_buf,save_buf,64);
	 
	switch(c_status) {
	    case 1:
		if(cipher_size != (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc3);
		}

		ecb_decry_proc((unsigned short *)cipher_text,MAX_TEXT_LEN/4,
                    (WORD*)plain_text,kd_buf);

		*plain_size=MAX_TEXT_LEN*2;

		break;

	    case 2:
		if(cipher_size != (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc3);
		}
	    ecb_decry_proc((unsigned short *)cipher_text,MAX_TEXT_LEN/4,
                    (WORD*)plain_text,kd_buf);
		*plain_size=MAX_TEXT_LEN*2;

		break;

	    case 3:
		if(cipher_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}

		ecb_decry_proc((unsigned short *)cipher_text,(WORD)(cipher_size/8),
                    inbuf,(WORD*)kd_buf);
	

		i=0;
		p=(unsigned char*)inbuf;
		tail_posi=cipher_size-1; //-2; debug by david , 20001.2.26
		while(p[tail_posi] != 1) {
			i++;
			if(i>=8)
			    return(0xc5);
			tail_posi=tail_posi -1;
		}
		*plain_size=tail_posi;
		memcpy(plain_text,(unsigned char*)inbuf,*plain_size);
		break;
	    case 4:
		if(cipher_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}

		ecb_decry_proc((unsigned short *)cipher_text,(WORD)(cipher_size/8),
                    inbuf,(WORD*)kd_buf);
	

		i=0;
		p=(unsigned char*)inbuf;
		tail_posi=cipher_size-1; //-2; debug by david , 20001.2.26
		while(p[tail_posi] != 1) {
			i++;
			if(i>=8)
			    return(0xc5);
			tail_posi=tail_posi -1;
		}
		*plain_size=tail_posi;
		memcpy(plain_text,(unsigned char*)inbuf,*plain_size);
		break;

	    default:
		return(0xff);
	}
	return 0;
}

 
BYTE generate_soft_mac_core(WORD c_status,BYTE *msg_text,WORD msg_size,
				BYTE *hash_text, WORD *hash_size,WORD *save_buf)
{
	WORD    *inbuf;
 	int     i;
	WORD    *word_point, cfb_block=8;

	int pad_bytes,pad_zero_no;
	int first_sndword,tail_bytes,indata_len;
	BYTE  last8bytes[100];
	WORD kd_buf[64];
        inbuf = static_inbuf;
	 
    memcpy(kd_buf,save_buf,64);
	
	switch(c_status) {
	    case 1:
		if(msg_size != (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc3);
		}
		memset(save_buf+32,0,64);
		ecb_mac_proc((unsigned short *)msg_text,(WORD)(MAX_TEXT_LEN/4),
                  save_buf+32,(WORD*)(kd_buf+8));
		break;

	    case 2:
		if(msg_size != (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc3);
		}
		ecb_mac_proc((unsigned short *)msg_text,MAX_TEXT_LEN/4,
            save_buf+32,kd_buf+8);

		break;

	    case 3:
		if(msg_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}
		cfb_block = 8;
                memcpy(hash_text,save_buf+32,8);
		pad_bytes = cfb_block - (msg_size % cfb_block);
		pad_zero_no = pad_bytes - 1;

		tail_bytes = msg_size;
		first_sndword = tail_bytes >> 1;

		inbuf[0]=1+((tail_bytes+pad_bytes) >>1);
		word_point = (WORD *)msg_text;
		for(i=0;i<first_sndword;i++)
			inbuf[i+2] = word_point[i];

		if((tail_bytes % 2) ==1) {
			last8bytes[0]=msg_text[msg_size - 1];
			last8bytes[1]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+2]=0;
		}
		else {
			last8bytes[0]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+1]=0;
		}

		indata_len=(pad_bytes+1) >> 1;
		word_point = (WORD *)last8bytes;
		for(i=0;i<indata_len;i++)
			inbuf[i+2+first_sndword] = word_point[i];
		ecb_mac_proc((unsigned short *)&inbuf[2],(WORD)((inbuf[0]-1)/4),
              (unsigned short*)hash_text,(WORD*)(kd_buf+8));

		break;

	    case 4:
		if(msg_size > (MAX_TEXT_LEN + MAX_TEXT_LEN)) {
			return(0xc4);
		}
		
		cfb_block = 8;
		memset(hash_text,0,8);
		pad_bytes = cfb_block - (msg_size % cfb_block);
		pad_zero_no = pad_bytes - 1;

		tail_bytes = msg_size;
		first_sndword=tail_bytes >> 1;

		inbuf[0]=1+((tail_bytes + pad_bytes) >>1);
		
		word_point = (WORD *)msg_text;
		for(i=0;i<first_sndword;i++)
			inbuf[i+2] = word_point[i];

		if((tail_bytes % 2) ==1) {
			last8bytes[0]=msg_text[msg_size - 1];
			last8bytes[1]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+2]=0;
		}
		else {
			last8bytes[0]=1;
			for(i=0;i<pad_zero_no;i++)
				last8bytes[i+1]=0;
		}

		indata_len=(pad_bytes+1) >> 1;
		word_point = (WORD *)last8bytes;
		for(i=0;i<indata_len;i++)
			inbuf[i+2+first_sndword] = word_point[i];

		ecb_mac_proc((unsigned short *)&inbuf[2],(WORD)((inbuf[0]-1)/4),
              (unsigned short*)hash_text,(WORD*)(kd_buf+8));
		break;
	    default:
		return(0Xff);
	}
	return 0;
}

#define MAX_BLOCK_LEN	4096
int  CSP_SDBIEncrypt(
        UCHAR  *output,
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen,
        UCHAR  * SDBIKey,
        ULONG  SDBIKeyLen
    )
{
	USHORT  retWord;
	USHORT  bytesLeft,bytesOver;
	USHORT inputLen_,outputLen_;	

	if (SDBIKeyLen!=16)
	{
		ErrorLocation;
		SetError(ERR_CODE_KEYLENGTH);
		return RTN_ERR;
	}
	
	if (inputLen <= MAX_BLOCK_LEN)
	{
		inputLen_=(USHORT)inputLen;
		retWord = Enc_Message_SDBI(ONLY,input,inputLen_,output,&outputLen_,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_ENCSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
	    else
		{
			*outputLen = (ULONG)outputLen_;
			return RTN_OK;
		}
	}

	else
	{
		bytesLeft = (USHORT)inputLen;
		bytesOver = 0;
		retWord = Enc_Message_SDBI(FIRST,input+bytesOver,MAX_BLOCK_LEN,output+bytesOver,&outputLen_,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_ENCSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += MAX_BLOCK_LEN;
		}
		while (bytesLeft > MAX_BLOCK_LEN)	
		{
		retWord = Enc_Message_SDBI(MID,input+bytesOver,MAX_BLOCK_LEN,output+bytesOver,&outputLen_,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_ENCSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += MAX_BLOCK_LEN;
		}	
		}
    
		retWord = Enc_Message_SDBI(LAST,input+bytesOver,bytesLeft,output+bytesOver,&outputLen_,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_ENCSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += outputLen_;
		}
		*outputLen = (ULONG)bytesOver;
		return RTN_OK;
	}
}

 
int  CSP_SDBIDecrypt(
        UCHAR  *output,
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen,
        UCHAR  * SDBIKey,
        ULONG  SDBIKeyLen
    )
{	
	USHORT  retWord;
	USHORT  bytesLeft,bytesOver;
	USHORT inputLen_,outputLen_;	

	if (SDBIKeyLen!=16)
	{
		ErrorLocation;
		SetError(ERR_CODE_KEYLENGTH);
		return RTN_ERR;
	}
	
	if (inputLen <= MAX_BLOCK_LEN)
	{
		inputLen_=(USHORT)inputLen;
		retWord = Dec_Message_SDBI(ONLY,output,&outputLen_,input,inputLen_,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_DECSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
	    else
		{
			*outputLen = (ULONG)outputLen_;
			return RTN_OK;
		}
	}

	else
	{
		bytesLeft = (USHORT)inputLen;
		bytesOver = 0;
		retWord = Dec_Message_SDBI(FIRST,output+bytesOver,&outputLen_,input+bytesOver,MAX_BLOCK_LEN,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_DECSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += MAX_BLOCK_LEN;
		}
		while (bytesLeft > MAX_BLOCK_LEN)	
		{
		retWord = Dec_Message_SDBI(MID,output+bytesOver,&outputLen_,input+bytesOver,MAX_BLOCK_LEN,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_DECSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += MAX_BLOCK_LEN;
		}	
		}
    
		retWord = Dec_Message_SDBI(LAST,output+bytesOver,&outputLen_,input+bytesOver,bytesLeft,SDBIKey);
		if (retWord)
		{
			SetError(ERR_CODE_DECSDBI);
			ErrorLocation;
			return RTN_ERR;
		}
		else
		{
			bytesLeft -= MAX_BLOCK_LEN;
			bytesOver += outputLen_;
		}
		*outputLen = (ULONG)bytesOver;
		return RTN_OK;
	}
}


/*

int CSP_i2d_RSAPublicKey(
        RSA_PUBLIC_KEY  *rsaPublicKey,
        UCHAR  *rsaPublicKeyDERString,
        ULONG  *rsaPublicKeyDERStringLen)
{
	return(i2d_RSAPublicKey(rsaPublicKey,
							rsaPublicKeyDERString,
							rsaPublicKeyDERStringLen));
}
int CSP_d2i_RSAPublicKey(
        UCHAR  *rsaPublicKeyDERString,
        ULONG  rsaPublicKeyDERStringLen,
        RSA_PUBLIC_KEY  *rsaPublicKey)
{
	return(d2i_RSAPublicKey(rsaPublicKeyDERString,
						rsaPublicKeyDERStringLen,rsaPublicKey));

}
int CSP_i2d_RSAPrivateKey(
        RSA_PRIVATE_KEY  *rsaPrivateKey,
        UCHAR  *rsaPrivateKeyDERString,
        ULONG  *rsaPrivateKeyDERStringLen)
{
     
	return(i2d_RSAPrivateKey(rsaPrivateKey,
						rsaPrivateKeyDERString,
						rsaPrivateKeyDERStringLen));
}
int CSP_d2i_RSAPrivateKey(
        UCHAR  *rsaPrivateKeyDERString,
        ULONG  rsaPrivateKeyDERStringLen,
        RSA_PRIVATE_KEY  *rsaPrivateKey)
{
 	return(d2i_RSAPrivateKey(rsaPrivateKeyDERString,
						rsaPrivateKeyDERStringLen,
						rsaPrivateKey));
}
*/

int  CSP_RSAPublicEncrypt(
		USHORT  keyNumber, 
        UCHAR  *output,
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen,
        RSA_PUBLIC_KEY  *rsaPublicKey
    )
{

	int ret_code;
	
	ret_code = RSAPublicEncrypt
		(output, outputLen, input, inputLen, (R_RSA_PUBLIC_KEY*)rsaPublicKey, &gRandomStruct);

	if (ret_code)
	{
	    ErrorLocation;
		return RTN_ERR; 
	}
	

	return RTN_OK;
}

/*Decrypt data use RSA PublicKey;*/

int  CSP_RSAPublicDecrypt(
		USHORT  keyNumber,
        UCHAR  *output,
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen,
        RSA_PUBLIC_KEY  * rsaPublicKey
    )
{
	int ret_code;

	ret_code = RSAPublicDecrypt 
	(output, outputLen, input, inputLen, (R_RSA_PUBLIC_KEY*)rsaPublicKey);
	if (ret_code)
	{
		ErrorLocation;
		return RTN_ERR;
	}
	return RTN_OK;
}

/* Encrypt data use RSA PrivateKey;*/

int  CSP_RSAPrivateEncrypt(
		USHORT  keyNumber,
        UCHAR  *output, 
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen, 
        RSA_PRIVATE_KEY  *rsaPrivateKey  
    )
{
	int ret_code;
	ret_code = RSAPrivateEncrypt 
		(output, outputLen, input, inputLen, (R_RSA_PRIVATE_KEY*)rsaPrivateKey);
	if (ret_code)
	{
		ErrorLocation;
		return RTN_ERR;
	}
	return RTN_OK;
}

/*Decrypt data use PrivateKey;*/

int  CSP_RSAPrivateDecrypt(
		USHORT  keyNumber,
        UCHAR  *output,
        ULONG  *outputLen,
        UCHAR  *input,
        ULONG  inputLen,
        RSA_PRIVATE_KEY  *rsaPrivateKey 
    )
{
	int ret_code;
	ret_code = 	RSAPrivateDecrypt 
		(output, outputLen, input, inputLen,(R_RSA_PRIVATE_KEY*)rsaPrivateKey);
	if (ret_code)
	{
		ErrorLocation;
		return RTN_ERR;
	}
	return RTN_OK;
}



static void InitRandomStruct (R_RANDOM_STRUCT * randomStruct)
{
	unsigned int bytesNeeded;
	long t_time;

	R_RandomInit (randomStruct);
	/*
	 *Initialize with time seed.
	 */
	while (1) {
		R_GetRandomBytesNeeded (&bytesNeeded, randomStruct);
		if (bytesNeeded == 0)
			break;

		time(&t_time);
		R_RandomUpdate (randomStruct,(unsigned char*) &t_time, sizeof(t_time));
	}
}


int  CSP_GenRandomBytes(
        UCHAR  *randomBytes,
        ULONG  randomBytesLen,
        UCHAR  *seedValue,
        ULONG  seedLen
     )
{
	int rtn_code;
	InitRandomStruct (&gRandomStruct);
	if(randomBytesLen <1 || randomBytesLen>256)
		return -1;
	rtn_code = R_GenerateBytes(randomBytes,randomBytesLen,&gRandomStruct);
 	if(rtn_code == 0) return RTN_OK;
	else return RTN_ERR;
}


int  CSP_GenRSAKeyPair(
        USHORT  keyNumber,/* if 0 then out the key else must be 1`5;*/
        RSA_PUBLIC_KEY  *rsaPublicKey,
        RSA_PRIVATE_KEY  *rsaPrivateKey
    )

{
  R_RSA_PROTO_KEY protoKey;
	
	if(keyNumber == 0){
	  protoKey.bits = 1024;
	  protoKey.useFermat4 = 1;
		
	  InitRandomStruct(&gRandomStruct);
	  return R_GeneratePEMKeys((R_RSA_PUBLIC_KEY*)rsaPublicKey, (R_RSA_PRIVATE_KEY*)rsaPrivateKey, &protoKey, &gRandomStruct);
    }
	return RTN_ERR;
}

/*Get out RSA PublicKey by Key Number;*/

int  CSP_GetRSAPublicKey(
        USHORT  keyNumber,
        RSA_PUBLIC_KEY  *rsaPublicKey
    )
{
    
	return RTN_ERR;
}

int  CSP_PutRSAKeyPair(
        USHORT  keyNumber,
        RSA_PUBLIC_KEY  *rsaPublicKey,
        RSA_PRIVATE_KEY  *rsaPrivateKey
    )
{
	return RTN_ERR;
}

int  CSP_InitEnvironment(void)
{
	return RTN_OK;
}

/*Clear Environment; Close Device...*/
int  CSP_ClearEnvironment(void)
{
	return RTN_OK;
}




