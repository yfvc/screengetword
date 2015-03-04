#include <stdio.h>
#include <memory.h>
#include <string.h>


#define FUNC_OK             0
#define FUNC_ERR            1

#define CPTR            *               
#define CCPTR           **
#define PUB_FUNC_RTN    uint32                  

#include "ca_common.h"
#include "csp_api.h"
#include "md2.h"/*../soft_crypt/*/
#include "md5.h"/*../soft_crypt/*/
#include "sha.h"/*../soft_crypt/*/

#include "common.h"
#include "g_val.h"
#include "asn1.h"

#include "cryptapi.h"
#include "rsaref.h"

static uint8  MD2_DIGEST_INFO[] = {
           0x30, 0x20, 
			     0x30, 0x0c, 
				       0x06, 0x08, 
					         0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x02,
                       0x05, 0x00,
                 0x04, 0x10 
};
 
static uint8  MD5_DIGEST_INFO[] = {
           0x30, 0x20, 
			     0x30, 0x0c, 
				       0x06, 0x08, 
					         0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
                       0x05, 0x00,
                 0x04, 0x10 
};

 
static uint8  SHA1_DIGEST_INFO[] = {
           0x30, 0x21, 
			     0x30, 0x09, 
				       0x06, 0x05, 
					         0x2b, 0x0e, 0x03, 0x02, 0x1a,
                       0x05, 0x00,
                 0x04, 0x14 
};


char *del_rn(char *pbuf);

CRET Crypt_Get_Version(char* version)
{
//	int rtn_code;

	strcpy(version, "2.0");

	return 0;
}



CRET Crypt_InitEnvironment(void)
{
	int rtn_code;

    rtn_code = CSP_InitEnvironment();
    if(rtn_code != RTN_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}

CRET Crypt_ClearEnvironment(void)
{
	int rtn_code;

    rtn_code = CSP_ClearEnvironment();
    if(rtn_code != RTN_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}


CRET CA_Crypt_Gen_Hash(
        int    uid_algorithm,
        unsigned char    *msg,
        int              msgSize,
        unsigned char    *hash,
		int              *hashSize)
{
	return Crypt_Gen_Hash(
        uid_algorithm,
        msg,
        msgSize,
        hash,
		hashSize);
}

int Crypt_Gen_Hash(
        int    uid_algorithm,
        unsigned char    *msg,
        int              msgSize,
        unsigned char    *hash,
		int              *hashSize)
{

	switch(uid_algorithm){
   	    case UID_md2WithRSAEncryption:
			MD2(msg,msgSize,hash);
			*hashSize = 16;
			break;
   	    case UID_md5WithRSAEncryption:
			MD5(msg,msgSize,hash);
			*hashSize = 16;
			break;
        case UID_md5:
			MD5(msg,msgSize,hash);
			*hashSize = 16;
			break;
   	    case UID_sha1WithRSASignature:
			SHA1(msg,msgSize,hash);
			*hashSize = 20;
			break;
   	    case UID_sha1:
			SHA1(msg,msgSize,hash);
			*hashSize = 20;
			break;
		default:
			return FUNC_ERR;
 	}

	return FUNC_OK;
}


CRET Crypt_Gen_Encoded_Hash(
        int    uid_algorithm,
        unsigned char    *msg,
        int              msgSize,
        unsigned char    *hash,
		int              *hashSize)
{
	unsigned char digestValue[32];
	int i;

	switch(uid_algorithm){
   	    case UID_md2WithRSAEncryption:
			MD2(msg,msgSize,digestValue);

			i = sizeof(MD2_DIGEST_INFO);
            memcpy(hash,MD2_DIGEST_INFO,i);
            memcpy(&hash[i],digestValue,16);

			*hashSize = i + 16;
			break;
   	    case UID_md5WithRSAEncryption:
			MD5(msg,msgSize,digestValue);

			i = sizeof(MD5_DIGEST_INFO);
            memcpy(hash,MD5_DIGEST_INFO,i);
            memcpy(&hash[i],digestValue,16);

			*hashSize = i + 16;
			break;
        case UID_md5:
			MD5(msg,msgSize,digestValue);

			i = sizeof(MD5_DIGEST_INFO);
            memcpy(hash,MD5_DIGEST_INFO,i);
            memcpy(&hash[i],digestValue,16);

			*hashSize = i + 16;
 			break;
   	    case UID_sha1WithRSASignature:
			SHA1(msg,msgSize,digestValue);

			i = sizeof(SHA1_DIGEST_INFO);
            memcpy(hash,SHA1_DIGEST_INFO,i);
            memcpy(&hash[i],digestValue,20);

			*hashSize = i + 20;
			break;
   	    case UID_sha1:
			SHA1(msg,msgSize,digestValue);

			i = sizeof(SHA1_DIGEST_INFO);
            memcpy(hash,SHA1_DIGEST_INFO,i);
            memcpy(&hash[i],digestValue,20);

			*hashSize = i + 20;
 			break;
		default:
			return FUNC_ERR;
 	}

	return FUNC_OK;
}


int c2d_RSA_PUBLIC_KEY(
        RSA_PUBLIC_KEY  *rsaPublicKey,
        uint8   *rsaPublicKeyDERString,
        uint32  *rsaPublicKeyDERStringLen)
{
/*
RSAPublicKey ::= SEQUENCE {
    modulus            INTEGER, -- n
    publicExponent     INTEGER  -- e -- }
 
typedef struct {
    uint16    bits;    
    UCHAR     modulus[MAX_RSA_MODULUS_LEN]; 
    UCHAR     publicExponent [MAX_RSA_MODULUS_LEN];  
}RSA_PUBLIC_KEY;
*/
 	DATA_BUFFER t_buf;
 	ASN1_SEQUENCE *p_head,*p_cur,*p_tail;
	uint8 buf[8]; 
	uint32 rtn_code;
	uint32 i,j,flag;

	p_head = 0;
	p_cur = 0;

	flag = 0;
	/* encode the n */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
			if(p_head != 0)
				free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPublicKey->modulus[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPublicKey->modulus[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
		if(p_head!=0)
			free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the e */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
			if(p_head!=0)
				free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPublicKey->publicExponent[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPublicKey->publicExponent[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
		if(p_head!=0)
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
 
    /* encode the whole sequence */
    rtn_code = ASN1_Encode_Sequence(
                   p_head,
				   &t_buf);
    if(rtn_code != FUNC_OK){
		if(p_head!=0)
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    *rsaPublicKeyDERStringLen = t_buf.length;
	memcpy(rsaPublicKeyDERString,t_buf.data,*rsaPublicKeyDERStringLen);

	rtn_code = free_ASN1_SEQUENCE(p_head);
    if(rtn_code != FUNC_OK){
		if(p_head!=0)
        free_DATA_BUFFER(&t_buf);
		return FUNC_ERR;
	}

    rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}

int d2c_RSA_PUBLIC_KEY(
        uint8  *rsaPublicKeyDERString,
        uint32 rsaPublicKeyDERStringLen,
        RSA_PUBLIC_KEY  *rsaPublicKey)
{
 	DATA_BUFFER t_buf;
 	ASN1_SEQUENCE *p_head,*p_cur;
	uint32 rtn_code;
	uint32 i;

	memset(rsaPublicKey,0,sizeof(RSA_PUBLIC_KEY));

	/* decode the sequence */
    t_buf.length = rsaPublicKeyDERStringLen;
	t_buf.data = rsaPublicKeyDERString;
    rtn_code = ASN1_Decode_Sequence(
 			       t_buf,
			       &p_head);
	p_cur = p_head;

    /* decode the n */
	if(p_cur == NULL){
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    if(t_buf.length <= 64){
		rsaPublicKey->bits = 512;
	}
	else{
        if(t_buf.length <= 128){
		    rsaPublicKey->bits = 1024;
		}
		else{
			if(t_buf.length <= 256){
		        rsaPublicKey->bits = 2048;
			}
			else{
		        return FUNC_ERR;
			}
		}
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPublicKey->modulus[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the e */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPublicKey->publicExponent[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	rtn_code = free_ASN1_SEQUENCE(p_head);
    if(rtn_code != FUNC_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}

int c2d_RSA_PRIVATE_KEY(
        RSA_PRIVATE_KEY  *rsaPrivateKey,
        uint8  *rsaPrivateKeyDERString,
        uint32 *rsaPrivateKeyDERStringLen)
{    
/*
RSAPrivateKey ::= SEQUENCE {
  version Version,
  modulus INTEGER, -- n
  publicExponent INTEGER, -- e
  privateExponent INTEGER, -- d
  prime1 INTEGER, -- p
  prime2 INTEGER, -- q
  exponent1 INTEGER, -- d mod (p-1)
  exponent2 INTEGER, -- d mod (q-1)
  coefficient INTEGER -- (inverse of q) mod p }

typedef struct rsa_pri_key{
uint16    bits;     
UCHAR     modulus[MAX_RSA_MODULUS_LEN]; 
UCHAR     publicExponent[MAX_RSA_MODULUS_LEN]; 
UCHAR     privateExponent[MAX_RSA_MODULUS_LEN];          
UCHAR     prime[2][MAX_RSA_PRIME_LEN];               
UCHAR     exponent[2][MAX_RSA_PRIME_LEN];               
UCHAR     coefficient [MAX_RSA_PRIME_LEN];               
UCHAR	  N[MAX_RSA_MODULUS_LEN];
UCHAR	  P[MAX_RSA_MODULUS_LEN];
UCHAR	  Q[MAX_RSA_MODULUS_LEN];
UCHAR	  d1[MAX_RSA_MODULUS_LEN];
UCHAR	  d2[MAX_RSA_MODULUS_LEN];
UCHAR	  P_[MAX_RSA_MODULUS_LEN];
UCHAR	  Q_[MAX_RSA_MODULUS_LEN];
UCHAR	  S[MAX_RSA_MODULUS_LEN];
UCHAR	  T[MAX_RSA_MODULUS_LEN];
} RSA_PRIVATE_KEY;
*/
	DATA_BUFFER t_buf;
 	ASN1_SEQUENCE *p_head,*p_cur,*p_tail;
	uint8 buf[8]; 
	uint32 rtn_code;
	uint32 i,j,flag;

    flag = 0;
	p_head = 0;
	p_cur = 0;

	/* encode the version */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

    buf[0] = 0;
 
	t_buf.length = 1;
	t_buf.data = buf;

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the modulus */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->modulus[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->modulus[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the publicExponent */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->publicExponent[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->publicExponent[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the privateExponent */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->privateExponent[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->privateExponent[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the prime[0] */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_PRIME_LEN;i++){
		if(rsaPrivateKey->prime[0][i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_PRIME_LEN - j;
	t_buf.data = &rsaPrivateKey->prime[0][j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the prime[1] */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_PRIME_LEN;i++){
		if(rsaPrivateKey->prime[1][i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_PRIME_LEN - j;
	t_buf.data = &rsaPrivateKey->prime[1][j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the exponent[0] */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_PRIME_LEN;i++){
		if(rsaPrivateKey->exponent[0][i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_PRIME_LEN - j;
	t_buf.data = &rsaPrivateKey->exponent[0][j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the exponent[1] */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_PRIME_LEN;i++){
		if(rsaPrivateKey->exponent[1][i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_PRIME_LEN - j;
	t_buf.data = &rsaPrivateKey->exponent[1][j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

	/* encode the coefficient */
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_PRIME_LEN;i++){
		if(rsaPrivateKey->coefficient[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_PRIME_LEN - j;
	t_buf.data = &rsaPrivateKey->coefficient[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
/***************************************/
/*	 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->N[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->N[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
 
 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->P[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->P[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->Q[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->Q[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->d1[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->d1[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->d2[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->d2[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->P_[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->P_[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->Q_[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->Q_[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}


 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->S[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->S[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

 
	if(flag == 0){
		flag = 1;
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
	        return FUNC_ERR;
		}

        p_head = p_tail;
        p_cur = p_tail;
	}
	else{
        rtn_code = new_ASN1_SEQUENCE(&p_tail);
        if(rtn_code != FUNC_OK){
            free_ASN1_SEQUENCE(p_head);
	        return FUNC_ERR;
		}

        p_cur->next = p_tail;
        p_cur = p_tail;
	}

	j = 0;
	for(i=0;i<MAX_RSA_MODULUS_LEN;i++){
		if(rsaPrivateKey->T[i] == 0){
			j++;
		}
		else{
			break;
		}
	}

	t_buf.length = MAX_RSA_MODULUS_LEN - j;
	t_buf.data = &rsaPrivateKey->T[j];

	if(t_buf.length == 0){
        buf[0] = 0;
	    t_buf.length = 1;
	    t_buf.data = buf;
    }

    rtn_code = ASN1_Encode_Integer(
                   t_buf,
			       &p_cur->data);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
*/
/****************************************/
    /* encode the whole sequence */
    rtn_code = ASN1_Encode_Sequence(
                   p_head,
				   &t_buf);
    if(rtn_code != FUNC_OK){
        free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    *rsaPrivateKeyDERStringLen = t_buf.length;
	memcpy(rsaPrivateKeyDERString,t_buf.data,*rsaPrivateKeyDERStringLen);

	rtn_code = free_ASN1_SEQUENCE(p_head);
    if(rtn_code != FUNC_OK){
        free_DATA_BUFFER(&t_buf);
		return FUNC_ERR;
	}

    rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}

int d2c_RSA_PRIVATE_KEY(
        uint8  *rsaPrivateKeyDERString,
        uint32 rsaPrivateKeyDERStringLen,
        RSA_PRIVATE_KEY  *rsaPrivateKey)
{
/*
typedef struct rsa_pri_key{
uint16    bits;     
UCHAR     modulus[MAX_RSA_MODULUS_LEN]; 
UCHAR     publicExponent[MAX_RSA_MODULUS_LEN]; 
UCHAR     privateExponent[MAX_RSA_MODULUS_LEN];          
UCHAR     prime[2][MAX_RSA_PRIME_LEN];               
UCHAR     exponent[2][MAX_RSA_PRIME_LEN];               
UCHAR     coefficient [MAX_RSA_PRIME_LEN];               
UCHAR	  N[MAX_RSA_MODULUS_LEN];
UCHAR	  P[MAX_RSA_MODULUS_LEN];
UCHAR	  Q[MAX_RSA_MODULUS_LEN];
UCHAR	  d1[MAX_RSA_MODULUS_LEN];
UCHAR	  d2[MAX_RSA_MODULUS_LEN];
UCHAR	  P_[MAX_RSA_MODULUS_LEN];
UCHAR	  Q_[MAX_RSA_MODULUS_LEN];
UCHAR	  S[MAX_RSA_MODULUS_LEN];
UCHAR	  T[MAX_RSA_MODULUS_LEN];
} RSA_PRIVATE_KEY;
*/
 	DATA_BUFFER t_buf;
 	ASN1_SEQUENCE *p_head,*p_cur;
	uint32 rtn_code;
	uint32 i;

	memset(rsaPrivateKey,0,sizeof(RSA_PRIVATE_KEY));

	/* decode the sequence */
    t_buf.length = rsaPrivateKeyDERStringLen;
	t_buf.data = rsaPrivateKeyDERString;
    rtn_code = ASN1_Decode_Sequence(
 			       t_buf,
			       &p_head);
	p_cur = p_head;

    /* decode the modulus */
	p_cur = p_cur->next; /* igore the version */
	if(p_cur == NULL){
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->modulus[i],t_buf.data,t_buf.length);

////////////////////////////  decode the bits  //////////////////
	if(t_buf.length <= 64){
		rsaPrivateKey->bits = 512;
	}
	else{
        if(t_buf.length <= 128){
		    rsaPrivateKey->bits = 1024;
		}
		else{
			if(t_buf.length <= 256){
		        rsaPrivateKey->bits = 2048;
			}
			else{
		        return FUNC_ERR;
			}
		}
	}
///////////////////////////////////////////////////////////////

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the publicExponent */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->publicExponent[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the privateExponent */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->privateExponent[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the prime[0] */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_PRIME_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->prime[0][i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the prime[1] */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_PRIME_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->prime[1][i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the exponent[0] */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_PRIME_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->exponent[0][i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the exponent[1] */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_PRIME_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->exponent[1][i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    /* decode the coefficient */
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_PRIME_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->coefficient[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
/***************************************/
/*     
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    if(t_buf.length <= 64){
		rsaPrivateKey->bits = 512;
	}
	else{
        if(t_buf.length <= 128){
		    rsaPrivateKey->bits = 1024;
		}
		else{
			if(t_buf.length <= 256){
		        rsaPrivateKey->bits = 2048;
			}
			else{
		        return FUNC_ERR;
			}
		}
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->N[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
 
    
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->P[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

   
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->Q[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->d1[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

  
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->d2[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

  
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->P_[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

   
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->Q_[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

   
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->S[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

   
	p_cur = p_cur->next;
	if(p_cur == NULL){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    rtn_code = ASN1_Decode_Integer(
		           p_cur->data,
                   &t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}

    i = MAX_RSA_MODULUS_LEN - t_buf.length;
    memcpy(&rsaPrivateKey->T[i],t_buf.data,t_buf.length);

	rtn_code = free_DATA_BUFFER(&t_buf);
    if(rtn_code != FUNC_OK){
		free_ASN1_SEQUENCE(p_head);
		return FUNC_ERR;
	}
*/
/******************************************/
	rtn_code = free_ASN1_SEQUENCE(p_head);
    if(rtn_code != FUNC_OK){
		return FUNC_ERR;
	}

	return FUNC_OK;
}

  
CRET Crypt_GenRSAKeyPair(
        char          *keyName,
        char          *password,
		unsigned char *DerPubkey,
		int			  *DerPubkeyLen,
		unsigned char *DerPrikey,
		int			  *DerPrikeyLen)
{
    int rtn_code;
    int keyNumber;
    RSA_PUBLIC_KEY  cardRsaPublicKey;
    RSA_PRIVATE_KEY cardRsaPrivateKey;
 
    keyNumber = 0;

	if(strcmp(keyName,"keypair.db1") == 0){
        keyNumber = 1;
	}

	if(strcmp(keyName,"keypair.db2") == 0){
        keyNumber = 2;
	}

	if(strcmp(keyName,"keypair.db3") == 0){
        keyNumber = 3;
	}

	if(strcmp(keyName,"keypair.db4") == 0){
        keyNumber = 4;
	}

	if(strcmp(keyName,"keypair.db5") == 0){
        keyNumber = 5;
	}
     
//printf("key filename %s\n\n", keyname);
    rtn_code = CSP_GenRSAKeyPair(
                   (USHORT)keyNumber,
                   &cardRsaPublicKey,
                   &cardRsaPrivateKey);
    if(rtn_code != 0){
        return FUNC_ERR;
    }



    rtn_code = c2d_RSA_PUBLIC_KEY(&cardRsaPublicKey,
                       DerPubkey,
			           DerPubkeyLen);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

	rtn_code = c2d_RSA_PRIVATE_KEY(
        &cardRsaPrivateKey,
        DerPrikey,
        DerPrikeyLen);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    if(keyNumber == 0){
		
		rtn_code = Crypt_SavePemKey2File(
		        keyName,
				password,
				/*&cardRsaPublicKey,*/ DerPubkey,
				/*sizeof(RSA_PUBLIC_KEY),*/ *DerPubkeyLen,
				/*&cardRsaPrivateKey,*/ DerPrikey,
				/*sizeof(RSA_PRIVATE_KEY));*/ *DerPrikeyLen);
		if(rtn_code != 0)
			return -1;
	}

	return FUNC_OK;
}

int Crypt_SavePemKey2File(
        char          *keyName,
        char          *password,
		unsigned char *DerPubkey,
		int			  DerPubkeyLen,
		unsigned char *DerPrikey,
		int			  DerPrikeyLen)
{

	FILE    *f;
	unsigned char pem_key[2048], hashPasswd[20], Enc_DerPubkey[2048];
	int rtn_code, pem_keySize, hashPasswdSize, Enc_DerPubkeySize;

    f = fopen(keyName,"wb");
	    if(f == NULL){
		    return FUNC_ERR;
        }

	rtn_code = Crypt_Gen_Hash(
				UID_md5,
				password,
				strlen(password),
				hashPasswd,
				&hashPasswdSize);
	if(rtn_code != 0)
		return -1;
/*
	rtn_code = iCrypt_Enc_Data_S(
				DerPubkey,
				DerPubkeyLen,
				Enc_DerPubkey,
				&Enc_DerPubkeySize,
				hashPasswd);
	if(rtn_code != 0)
		return -1;
*/	    
	rtn_code = DER2PEM_data_format(
                   DerPubkey,//Enc_DerPubkey, 
                   DerPubkeyLen,//Enc_DerPubkeySize,
                   pem_key,
                   &pem_keySize,
                   PublicKey_Begin_String, 
                   PublicKey_End_String);
    if(rtn_code != FUNC_OK){
        printf("DER2PEM_data_format error...\n");
        return -1;
    }
	fwrite(pem_key,1,pem_keySize,f);

	rtn_code = iCrypt_Enc_Data_S(
				DerPrikey,
				DerPrikeyLen,
				Enc_DerPubkey,
				&Enc_DerPubkeySize,
				hashPasswd);
	if(rtn_code != 0)
		return -1;
			

	rtn_code = DER2PEM_data_format(
                   Enc_DerPubkey, 
                   Enc_DerPubkeySize,
                   pem_key,
                   &pem_keySize,
                   PrivateKey_Begin_String, 
                   PrivateKey_End_String);
    if(rtn_code != FUNC_OK){
        printf("DER2PEM_data_format error...\n");
        return -1;
    }
	fwrite(pem_key,1,pem_keySize,f);

	fclose(f);

	return 0;
}
	

CRET Crypt_ChgKeyPasswd(
        char          *keyName,
		char          *OldPasswd,
		char          *NewPasswd)
{
	unsigned char DerPubKey[1024], DerPrikey[1024];
	int rtn_code, DerPubKeyLen, DerPrikeyLen, keyNumber;
//    RSA_PUBLIC_KEY  cardRsaPublicKey;
//    RSA_PRIVATE_KEY cardRsaPrivateKey;

    keyNumber = 0;

	if(strcmp(keyName,"keypair.db1") == 0){
        keyNumber = 1;
	}

	if(strcmp(keyName,"keypair.db2") == 0){
        keyNumber = 2;
	}

	if(strcmp(keyName,"keypair.db3") == 0){
        keyNumber = 3;
	}

	if(strcmp(keyName,"keypair.db4") == 0){
        keyNumber = 4;
	}

	if(strcmp(keyName,"keypair.db5") == 0){
        keyNumber = 5;
	}
     
    if(keyNumber == 0){

		rtn_code =  RestoreKeyFromFile(
			keyName,
			OldPasswd,
			DerPubKey,//&cardRsaPublicKey,
			&DerPubKeyLen,
			DerPrikey,//&cardRsaPrivateKey,
			&DerPrikeyLen);
		if(rtn_code != 0)
			return -1;
		
		rtn_code = Crypt_SavePemKey2File(
		        keyName,
				NewPasswd,
				DerPubKey,//&cardRsaPublicKey,
				DerPubKeyLen,//sizeof(RSA_PUBLIC_KEY),
				DerPrikey,//&cardRsaPrivateKey,
				DerPrikeyLen);//sizeof(RSA_PRIVATE_KEY));
		if(rtn_code != 0)
			return -1;
	}

	return 0;
}		

CRET Crypt_Decode_Pubkey_Info(
		unsigned char	*DerPublicKeyInfo,
		int				DerPublicKeyInfoSize,
		int				*bits,
		unsigned char	*modulus,
		unsigned char   *publicExponent)
{
//    unsigned char subjectPublicKeyInfo[512];
//    int  subjectPublicKeyInfoSize;
    RSA_PUBLIC_KEY  rsaPublicKey;

	int rtn_code;

/*
typedef struct rsa_pub_key{
    uint16       bits;    // length in bits of modulus 
	UCHAR     modulus[MAX_RSA_MODULUS_LEN]; 
                   // modulus  ----- n 
	UCHAR     publicExponent[MAX_RSA_MODULUS_LEN];  
                   // public exponent  -----  e  
} RSA_PUBLIC_KEY;
*/
    
	rtn_code = d2c_RSA_PUBLIC_KEY(
                   DerPublicKeyInfo,
                   DerPublicKeyInfoSize,
                   &rsaPublicKey);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

	*bits = rsaPublicKey.bits;
	memcpy(modulus, rsaPublicKey.modulus, sizeof(rsaPublicKey.modulus));
	memcpy(publicExponent, rsaPublicKey.publicExponent, sizeof(rsaPublicKey.publicExponent));

	return 0;	
}

CRET Crypt_Decode_Prikey_Info(
		unsigned char	*DerPrivateKeyInfo,
		int				DerPrivateKeyInfoSize,
		int				*bits,
		unsigned char	*modulus,
		unsigned char   *publicExponent,
		unsigned char   *privateExponent,
		unsigned char   *prime,
		unsigned char   *exponent,
		unsigned char   *coefficient)
{
//	unsigned char DerPrikey[1024];
//	int DerPrikeyLen;
    RSA_PRIVATE_KEY  cardRsaPrivateKey;
	int rtn_code;

	rtn_code = d2c_RSA_PRIVATE_KEY(
			DerPrivateKeyInfo,
			DerPrivateKeyInfoSize,
			&cardRsaPrivateKey);
		if(rtn_code != 0)
			return -1;

	*bits = cardRsaPrivateKey.bits;
	memcpy(modulus, cardRsaPrivateKey.modulus, sizeof(cardRsaPrivateKey.modulus));
	memcpy(publicExponent, cardRsaPrivateKey.publicExponent, sizeof(cardRsaPrivateKey.publicExponent));
	memcpy(privateExponent, cardRsaPrivateKey.privateExponent, sizeof(cardRsaPrivateKey.privateExponent));
	memcpy(prime, cardRsaPrivateKey.prime[2], sizeof(cardRsaPrivateKey.prime[2]));
	memcpy(exponent, cardRsaPrivateKey.exponent[2], sizeof(cardRsaPrivateKey.exponent[2]));
	memcpy(coefficient, cardRsaPrivateKey.coefficient, sizeof(cardRsaPrivateKey.coefficient));

	return 0;
}


CRET Crypt_RestorePublicKey(
        char          *keyName,
		char          *password,
		unsigned char *DerPubkey,
		int			  *DerPubkeyLen)
{
	//int rtn_code;
	unsigned char DerPriKey[1024];
	int DerPriKeyLen;

	return RestoreKeyFromFile(
			keyName,
			password,
			DerPubkey,
			DerPubkeyLen,
			DerPriKey,
			&DerPriKeyLen);
}	

CRET Crypt_RestorePriKey(
        char          *keyName,
		char          *password,
		unsigned char *DerPrikey,
		int			  *DerPrikeyLen)
{
	//int rtn_code;
	unsigned char DerPubKey[1024];
	int DerPubKeyLen;

	return RestoreKeyFromFile(
			keyName,
			password,
			DerPubKey,
			&DerPubKeyLen,
			DerPrikey,
			DerPrikeyLen);
}		


int RestoreKeyFromFile(
        char          *keyName,
        char          *password,
		unsigned char *DerPubkey,
		int			  *DerPubkeyLen,
		unsigned char *DerPrikey,
		int			  *DerPrikeyLen)
{

	FILE    *f;
	unsigned char pem_key[2048], hashPasswd[20], Dec_Derkey[1024], buffer[128];
	int rtn_code,  hashPasswdSize, Dec_DerkeySize, ptr_offset;

    ptr_offset=0;

	f = fopen(keyName,"r");
	    if(f == NULL){
			printf("open failed! keyname=%s\n", keyName);
		    return FUNC_ERR;
        }

	do{
		fgets(buffer, sizeof(buffer), f);
//printf("len=%d, buffer:%s\n", strlen(buffer), buffer);
                del_rn(buffer);
//printf("after del_rn, len=%d, buffer:%s\n", strlen(buffer), buffer);
		memcpy(pem_key+ptr_offset, buffer,strlen(buffer));
		ptr_offset += strlen(buffer);
	}
	while(strcmp(buffer,PublicKey_End_String2));

    rtn_code = PEM2DER_data_format(
                       pem_key,
                       strlen(pem_key),
                       DerPubkey,//Dec_Derkey, 
                       DerPubkeyLen);//&Dec_DerkeySize);
    if(rtn_code != FUNC_OK){
        printf("PEM2DER_data_format error...\n");
		fclose(f);
        return -1;
    }
	rtn_code = Crypt_Gen_Hash(
				UID_md5,
				password,
				strlen(password),
				hashPasswd,
				&hashPasswdSize);
	if(rtn_code != 0){
		fclose(f);
		return -1;
	}
/*
	rtn_code = iCrypt_Dec_Data_S(
				Dec_Derkey,
				Dec_DerkeySize,
				DerPubkey,
				DerPubkeyLen,
				hashPasswd);
	if(rtn_code != 0){
		fclose(f);
		return -1;
	}
*/
	ptr_offset = 0;
	do{
		fgets(buffer, sizeof(buffer), f);
//printf("len=%d, buffer=%s\n", strlen(buffer), buffer);
                del_rn(buffer);
//printf("after del_rn:len=%d, buffer=%s\n", strlen(buffer), buffer);
		memcpy(pem_key+ptr_offset, buffer,strlen(buffer));
		ptr_offset += strlen(buffer);
	}
	while(strcmp(buffer,PrivateKey_End_String2));
	fclose(f);

    rtn_code = PEM2DER_data_format(
                       pem_key,
                       strlen(pem_key),
                       Dec_Derkey,
     	  			   &Dec_DerkeySize);
    if(rtn_code != FUNC_OK){
        printf("PEM2DER_data_format error...\n");
        return -1;
    }

	rtn_code = iCrypt_Dec_Data_S(
				Dec_Derkey,
				Dec_DerkeySize,
				DerPrikey,
				DerPrikeyLen,
				hashPasswd);
	if(rtn_code != 0)
		return -1;

	return 0;
}

CRET Crypt_GetRSAPublicKey(
        char           *keyName,
		char			*passwd,
        unsigned char  *rsaPublicKey,
        int            *rsaPublicKeySize)
{
    int rtn_code;
    int keyNumber;
//    RSA_PUBLIC_KEY  cardRsaPublicKey;

    keyNumber = 0;

	if(strcmp(keyName,"keypair.db1") == 0){
        keyNumber = 1;
	}

	if(strcmp(keyName,"keypair.db2") == 0){
        keyNumber = 2;
	}

	if(strcmp(keyName,"keypair.db3") == 0){
        keyNumber = 3;
	}

	if(strcmp(keyName,"keypair.db4") == 0){
        keyNumber = 4;
	}

	if(strcmp(keyName,"keypair.db5") == 0){
        keyNumber = 5;
	}

    if(keyNumber == 0){
	rtn_code = Crypt_RestorePublicKey(
				keyName,
				passwd,
				rsaPublicKey,//&cardRsaPublicKey,
				rsaPublicKeySize);
      if(rtn_code != 0)
            return FUNC_ERR;
        
    }
/*
    d2c_RSA_PUBLIC_KEY(rsaPublicKey,*rsaPublicKeySize,
                       &cardRsaPublicKey);

    c2d_RSA_PUBLIC_KEY(&cardRsaPublicKey,
                       rsaPublicKey,
			           rsaPublicKeySize);

*/
    return FUNC_OK;
}

CRET CA_Crypt_Gen_Signature( 
        int    uid_algorithm,
		char   *keyName,
		char   *password,
        unsigned char  *msg,
        int            msgSize,
        unsigned char  *sig,
		int            *sigSize)
{
	return     Crypt_Gen_Signature( 
			    uid_algorithm,
			    keyName,
				password,
				msg,
                msgSize,
				sig,
				sigSize);
}

int Crypt_Gen_Signature( 
        int    uid_algorithm,
		char   *keyName,
		char   *password,
        unsigned char  *msg,
        int            msgSize,
        unsigned char  *sig,
		int            *sigSize)
{
    int rtn_code;
    int keyNumber;
	uint32 hashSize;
    UCHAR hash[64],output[128]; 
    ULONG outputLen;
	unsigned char DerPrikey[1024];
	int DerPrikeyLen;
//    RSA_PUBLIC_KEY  cardRsaPublicKey;
    RSA_PRIVATE_KEY  cardRsaPrivateKey;


    keyNumber = 0;

	if(strcmp(keyName,"keypair.db1") == 0){
        keyNumber = 1;
	}

	if(strcmp(keyName,"keypair.db2") == 0){
        keyNumber = 2;
	}

	if(strcmp(keyName,"keypair.db3") == 0){
        keyNumber = 3;
	}

	if(strcmp(keyName,"keypair.db4") == 0){
        keyNumber = 4;
	}

	if(strcmp(keyName,"keypair.db5") == 0){
        keyNumber = 5;
	}

    if(keyNumber == 0){
		rtn_code = Crypt_RestorePriKey(
			keyName,
			password,
			DerPrikey,//&cardRsaPrivateKey,
			&DerPrikeyLen);
		if(rtn_code != 0)
			return -1;
//*
		rtn_code = d2c_RSA_PRIVATE_KEY(
			DerPrikey,
			DerPrikeyLen,
			&cardRsaPrivateKey);
		if(rtn_code != 0)
			return -1;
//*/
  }

/////////////  it is tmp codes, maybe modify ....... ///////////////////////////
//	cardRsaPrivateKey.bits = 1024;
////////////////////////////////////////////////////////////////////////////////

	rtn_code = Crypt_Gen_Encoded_Hash(
		           uid_algorithm,
                   msg,
                   msgSize,
				   hash,
				   &hashSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    rtn_code = CSP_RSAPrivateEncrypt(
	               (USHORT)keyNumber,
                   output, 
                   &outputLen,
                   hash,
                   (ULONG)hashSize, 
                   &cardRsaPrivateKey);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }
 
    memcpy(sig,output,outputLen);
	*sigSize = (uint32)outputLen;

    return FUNC_OK;
}


CRET Crypt_Ver_Signature(
        int            uid_algorithm,
        unsigned char  *rsaPublicKey,
        int            rsaPublicKeySize,
        unsigned char  *msg,
        int            msgSize,
        unsigned char  *sig,
		int            sigSize)
{
    int rtn_code;
	uint32 hashSize;
    UCHAR hash[64],output[128]; 
    ULONG  outputLen;
    USHORT keyNumber;
    RSA_PUBLIC_KEY cardRsaPublicKey;


    d2c_RSA_PUBLIC_KEY(rsaPublicKey,rsaPublicKeySize,
                       &cardRsaPublicKey);

	rtn_code = Crypt_Gen_Encoded_Hash(
		           uid_algorithm,
                   msg,
                   msgSize,
				   hash,
				   &hashSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    keyNumber = 0;
    rtn_code = CSP_RSAPublicDecrypt(
		           (USHORT)keyNumber,
                    output,
                    &outputLen,
                    sig,
                    (ULONG)sigSize,
                    &cardRsaPublicKey); 
     if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }


    if(memcmp(hash,output,hashSize) != 0){
        return FUNC_ERR;
    }

    return FUNC_OK;
}


CRET 
Crypt_Get_PubKey_FromCert(
    unsigned char   *der_cert,
    int             der_certSize,
    unsigned char   *subjectPublicKeyInfo,
    int             *subjectPublicKeyInfoSize)
{                          
/*
    int version,serialNumberSize,signatureAlgorithm,parametersSize;
    unsigned char  serialNumber[256],parameters[64];
    char issuer_countryName[64],issuer_organizationName[64];
    char issuer_organizationalUnitname1[64],issuer_organizationalUnitname2[64];
    char issuer_organizationalUnitname3[64],issuer_stateOrProvinceName[64];
    char issuer_commonName[64],issuer_localityName[64];
    char issuer_title[64],issuer_surname[64];
    char issuer_givenName[64],issuer_initials[64];
    char issuer_email[64],issuer_postalAddress[64];
    char issuer_postalCode[64],issuer_postalOfficeBox[64];
    char issuer_telephoneNumber[64],issuer_telexNumber[64];
    char notBefore[64],notAfter[64];
    char subject_countryName[64],subject_organizationName[64];
    char subject_organizationalUnitname1[64],subject_organizationalUnitname2[64];
    char subject_organizationalUnitname3[64],subject_stateOrProvinceName[64];
    char subject_commonName[64],subject_localityName[64];
    char subject_title[64],subject_surname[64];
    char subject_givenName[64],subject_initials[64];
    char subject_email[64],subject_postalAddress[64];
    char subject_postalCode[64],subject_postalOfficeBox[64];
    char subject_telephoneNumber[64],subject_telexNumber[64];
 
    unsigned char issuerUniqueID[64];
    unsigned char subjectUniqueID[64];
    int  issuerUniqueIDSize,subjectUniqueIDSize;
    unsigned int   extensionsFlag,criticalFlag;
 
    char authKeyId_countryName[64],authKeyId_organizationName[64];
    char authKeyId_organizationalUnitname1[64],authKeyId_organizationalUnitname2[64];
    char authKeyId_organizationalUnitname3[64],authKeyId_stateOrProvinceName[64];
    char authKeyId_commonName[64],authKeyId_localityName[64],authKeyId_title[64];
    char authKeyId_surname[64],authKeyId_givenName[64];
    char authKeyId_initials[64],authKeyId_email[64];
    char authKeyId_postalAddress[64],authKeyId_postalCode[64];
    char authKeyId_postalOfficeBox[64],authKeyId_telephoneNumber[64];
    char authKeyId_telexNumber[64];

    unsigned char authKeyId_serialNumber[64];
    int authKeyId_serialNumberSize;

    unsigned char policyId1[64],policyId2[64],policyId3[64],policyId4[64];
    int policyIdSize1,policyIdSize2,policyIdSize3,policyIdSize4;
    char policyUrl1[64],policyUrl2[64],policyUrl3[64],policyUrl4[64];
    char PrivateKeyUsagePeriod_notBefore[64];
    char PrivateKeyUsagePeriod_notAfter[64];
    int  BasicConstraints_cA,BasicConstraints_pathLenConstraint;
   
    unsigned int   KeyUsage,neCertType,setCertType;
    int  self_certClass;
    char revocationURL[64];

    unsigned char  hashedRootKey[64];
    int            hashedRootKeySize;
*/

    int version,serialNumberSize,signatureAlgorithm,parametersSize;
    unsigned char  serialNumber[256],parameters[64];
    char issuer_countryName[64],issuer_organizationName[64];
    char issuer_organizationalUnitname1[64];
    char issuer_stateOrProvinceName[64];
    char issuer_commonName[64],issuer_localityName[64];
    char issuer_title[64],issuer_surname[64];
    char issuer_givenName[64],issuer_initials[64];
    char issuer_email[64],issuer_postalAddress[64];
    char issuer_postalCode[64],issuer_postalOfficeBox[64];
    char issuer_telephoneNumber[64],issuer_telexNumber[64];
    char notBefore[64],notAfter[64];
    char subject_countryName[64],subject_organizationName[64];
    char subject_organizationalUnitname1[64];
    char subject_stateOrProvinceName[64];
    char subject_commonName[64],subject_localityName[64];
    char subject_title[64],subject_surname[64];
    char subject_givenName[64],subject_initials[64];
    char subject_email[64],subject_postalAddress[64];
    char subject_postalCode[64],subject_postalOfficeBox[64];
    char subject_telephoneNumber[64],subject_telexNumber[64];
 
    unsigned char issuerUniqueID[64];
    unsigned char subjectUniqueID[64];
    int  issuerUniqueIDSize,subjectUniqueIDSize;
    unsigned int   extensionsFlag,criticalFlag;
 
    char authKeyId_countryName[64],authKeyId_organizationName[64];
    char authKeyId_organizationalUnitname1[64];
    char authKeyId_stateOrProvinceName[64];
    char authKeyId_commonName[64],authKeyId_localityName[64],authKeyId_title[64];
    char authKeyId_surname[64],authKeyId_givenName[64];
    char authKeyId_initials[64],authKeyId_email[64];
    char authKeyId_postalAddress[64],authKeyId_postalCode[64];
    char authKeyId_postalOfficeBox[64],authKeyId_telephoneNumber[64];
    char authKeyId_telexNumber[64];

    unsigned char authKeyId_serialNumber[64];
    int authKeyId_serialNumberSize;

    unsigned char policyId1[64],policyId2[64],policyId3[64],policyId4[64];
    int policyIdSize1,policyIdSize2,policyIdSize3,policyIdSize4;
    char policyUrl1[64],policyUrl2[64],policyUrl3[64],policyUrl4[64];
    char PrivateKeyUsagePeriod_notBefore[64];
    char PrivateKeyUsagePeriod_notAfter[64];
    int  BasicConstraints_cA,BasicConstraints_pathLenConstraint;
   
    unsigned int   KeyUsage,neCertType,setCertType;
    int  self_certClass;
    char revocationURL[64];

    unsigned char  hashedRootKey[64];
    int            hashedRootKeySize;

    int  rtn_code;

 
    rtn_code = Crypt_Decode_Cert(
                   der_cert,
                   der_certSize,

                   &version,
                   serialNumber,
                   &serialNumberSize,
                   &signatureAlgorithm,
                   parameters,
                   &parametersSize,
                   issuer_countryName,
                   issuer_organizationName,
                   issuer_organizationalUnitname1,
//                   issuer_organizationalUnitname2,
//                   issuer_organizationalUnitname3,
                   issuer_stateOrProvinceName,
                   issuer_commonName,
                   issuer_localityName,
                   issuer_title,
                   issuer_surname,
                   issuer_givenName,
                   issuer_initials,
                   issuer_email,
                   issuer_postalAddress,
                   issuer_postalCode,
                   issuer_postalOfficeBox,
                   issuer_telephoneNumber,
                   issuer_telexNumber,
                   notBefore,   
                   notAfter,     
                   subject_countryName,
                   subject_organizationName,
                   subject_organizationalUnitname1,
//                   subject_organizationalUnitname2,
//                   subject_organizationalUnitname3,
                   subject_stateOrProvinceName,
                   subject_commonName,
                   subject_localityName,
                   subject_title,
                   subject_surname,
                   subject_givenName,
                   subject_initials,
                   subject_email,
                   subject_postalAddress,
                   subject_postalCode,
                   subject_postalOfficeBox,
                   subject_telephoneNumber,
                   subject_telexNumber,
                   subjectPublicKeyInfo,
                   subjectPublicKeyInfoSize,
                   issuerUniqueID, 
                   &issuerUniqueIDSize,
                   subjectUniqueID,
                   &subjectUniqueIDSize,
                   &extensionsFlag,
                   &criticalFlag,
                   authKeyId_countryName,
                   authKeyId_organizationName,
                   authKeyId_organizationalUnitname1,
//                   authKeyId_organizationalUnitname2,
//                   authKeyId_organizationalUnitname3,
                   authKeyId_stateOrProvinceName,
                   authKeyId_commonName,
                   authKeyId_localityName,
                   authKeyId_title,
                   authKeyId_surname,
                   authKeyId_givenName,
                   authKeyId_initials,
                   authKeyId_email,
                   authKeyId_postalAddress,
                   authKeyId_postalCode,
                   authKeyId_postalOfficeBox,
                   authKeyId_telephoneNumber,
                   authKeyId_telexNumber,
                   authKeyId_serialNumber,
                   &authKeyId_serialNumberSize,
                   policyId1,
                   &policyIdSize1,
                   policyUrl1,
                   policyId2,
                   &policyIdSize2,
                   policyUrl2,
                   policyId3,
                   &policyIdSize3,
                   policyUrl3,
                   policyId4,
                   &policyIdSize4,
                   policyUrl4,
                   PrivateKeyUsagePeriod_notBefore,   
                   PrivateKeyUsagePeriod_notAfter,     
                   &BasicConstraints_cA,
                   &BasicConstraints_pathLenConstraint,
                   &KeyUsage,
                   &neCertType,
                   &setCertType,
                   hashedRootKey,
                   &hashedRootKeySize,

                   &self_certClass,
                   revocationURL);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    return FUNC_OK; 
}

 
CRET 
Crypt_Ver_Signature_ByCert(
    int             uid_algorithm,
    unsigned char   *der_cert,
    int             der_certSize,
    unsigned char   *msg,
    int             msgSize,
    unsigned char   *sig,
    int             sigSize)
{
    unsigned char subjectPublicKeyInfo[512];
    int  subjectPublicKeyInfoSize;
    int  rtn_code;


    rtn_code = Crypt_Get_PubKey_FromCert(
                    der_cert,
                    der_certSize,
                    subjectPublicKeyInfo,
                    &subjectPublicKeyInfoSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    rtn_code = Crypt_Ver_Signature(
                   uid_algorithm,
                   subjectPublicKeyInfo,
                   subjectPublicKeyInfoSize,
                   msg,
                   msgSize,
                   sig,
		           sigSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    return FUNC_OK;
}

int iCrypt_Enc_Data_S(
	unsigned char  *indata,
    int            indataSize,
    unsigned char  *outdata,
    int            *outdataSize,
	unsigned char  *sdbiKey)
{
    int rtn_code = CSP_SDBIEncrypt(
                   outdata,
                   outdataSize,
                   indata,
                   (ULONG)indataSize,
                   sdbiKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }
	return FUNC_OK;
}

CRET Crypt_Enc_Data_S(
	unsigned char  *indata,
    int            indataSize,
    unsigned char  *outdata,
    int            *outdataSize,
	unsigned char  *sdbiKey)
{

    int rtn_code = CSP_SDBIEncrypt(
                   outdata,
                   outdataSize,
                   indata,
                   (ULONG)indataSize,
                   sdbiKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }
	return FUNC_OK;
}

CRET
Crypt_Enc_Data(
    unsigned char  *indata,
    int            indataSize,
    unsigned char  *outdata,
    int            *outdataSize,
    unsigned char  *der_cert,
    int            der_certSize)
{
    unsigned char subjectPublicKeyInfo[512];
    int  subjectPublicKeyInfoSize;
    RSA_PUBLIC_KEY  rsaPublicKey;
    UCHAR  SDBIKey[32],seedValue[32];
    UCHAR  enc_SDBIKey[256];
    ULONG  SDBIKeyLen,seedLen;

    int rtn_code;


    rtn_code = Crypt_Get_PubKey_FromCert(
                    der_cert,
                    der_certSize,
                    subjectPublicKeyInfo,
                    &subjectPublicKeyInfoSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    seedLen = 16;
    rtn_code = CSP_GenRandomBytes(
                   SDBIKey,
                   16,
                   seedValue,
                   seedLen);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }
               
    rtn_code = d2c_RSA_PUBLIC_KEY(
                   subjectPublicKeyInfo,
                   subjectPublicKeyInfoSize,
                   &rsaPublicKey);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    rtn_code = CSP_RSAPublicEncrypt(
		           0, 
                   enc_SDBIKey,
                   &SDBIKeyLen,
                   SDBIKey,
                   16,
                   &rsaPublicKey);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }

    memcpy(outdata,enc_SDBIKey,SDBIKeyLen);

    rtn_code = CSP_SDBIEncrypt(
                   &outdata[SDBIKeyLen],
                   &seedLen,
                   indata,
                   (ULONG)indataSize,
                   SDBIKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }

    *outdataSize = SDBIKeyLen + seedLen;
   
    return FUNC_OK;
}

int iCrypt_Dec_Data_S(
      unsigned char  *indata,
      int           indataSize,
      unsigned char  *outdata,
      int           *outdataSize,
      unsigned char  *sdbiKey)
{
    int rtn_code = CSP_SDBIDecrypt(
                   outdata,
                   outdataSize,
                   indata,
                   indataSize,
                   sdbiKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }

    return FUNC_OK;
}


CRET  Crypt_Dec_Data_S(
      unsigned char  *indata,
      int           indataSize,
      unsigned char  *outdata,
      int           *outdataSize,
      unsigned char  *sdbiKey)
{
    int rtn_code = CSP_SDBIDecrypt(
                   outdata,
                   outdataSize,
                   indata,
                   indataSize,
                   sdbiKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }

    return FUNC_OK;
}

CRET  Crypt_Dec_Data(
      unsigned char  *indata,
      int           indataSize,
      unsigned char  *outdata,
      int           *outdataSize,
      char          *keyName,
      char          *password)
{
    int rtn_code;
    int keyNumber;
    UCHAR SDBIKey[128]; 
    ULONG outputLen;

	unsigned char DerPrikey[1024];
	int DerPrikeyLen;

//    RSA_PUBLIC_KEY  cardRsaPublicKey;
    RSA_PRIVATE_KEY  cardRsaPrivateKey;


    keyNumber = 0;

	if(strcmp(keyName,"keypair.db1") == 0){
        keyNumber = 1;
	}

	if(strcmp(keyName,"keypair.db2") == 0){
        keyNumber = 2;
	}

	if(strcmp(keyName,"keypair.db3") == 0){
        keyNumber = 3;
	}

	if(strcmp(keyName,"keypair.db4") == 0){
        keyNumber = 4;
	}

	if(strcmp(keyName,"keypair.db5") == 0){
        keyNumber = 5;
	}


    if(keyNumber == 0){
		rtn_code = Crypt_RestorePriKey(
			keyName,
			password,
			DerPrikey,//&cardRsaPrivateKey,
			&DerPrikeyLen);
		if(rtn_code != 0)
			return -1;
//*
		rtn_code = d2c_RSA_PRIVATE_KEY(
			DerPrikey,
			DerPrikeyLen,
			&cardRsaPrivateKey);
		if(rtn_code != 0)
			return -1;
//*/  
	}
///////////////////////// tmp codes, maybe modify....................
//	cardRsaPrivateKey.bits = 1024;
/////////////////////////////////////////////////////////////////////

    rtn_code = CSP_RSAPrivateDecrypt(
		           keyNumber,
                   SDBIKey,
                   &outputLen,
                   indata,
                   128,
                   &cardRsaPrivateKey);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }
    
    rtn_code = CSP_SDBIDecrypt(
                   outdata,
                   &outputLen,
                   &indata[128],
                   indataSize-128,
                   SDBIKey,
                   16);
    if(rtn_code != RTN_OK){
        return FUNC_ERR;
    }

    *outdataSize = outputLen;

    return FUNC_OK;
}

CRET Crypt_Rsa_PrivateKeyDec(
	char *keyName,
	char *password,
	unsigned char *cipher,
	int cipherLen,
	unsigned char *msg,
	int *msgLen)
{
    int rtn_code;
    int keyNumber;
 
	unsigned char DerPrikey[1024];
	int DerPrikeyLen;

//    RSA_PUBLIC_KEY  cardRsaPublicKey;
    RSA_PRIVATE_KEY  cardRsaPrivateKey;

		rtn_code = Crypt_RestorePriKey(
			keyName,
			password,
			DerPrikey,//&cardRsaPrivateKey,
			&DerPrikeyLen);
		if(rtn_code != 0)
			return -1;
		rtn_code = d2c_RSA_PRIVATE_KEY(
			DerPrikey,
			DerPrikeyLen,
			&cardRsaPrivateKey);
		if(rtn_code != 0)
			return -1;

    /*
	rtn_code = CSP_RSAPrivateDecrypt(
		           0,
                   msg,
                   &msgLen,
                   cipher,
                   cipherLen,
                   &cardRsaPrivateKey);
	*/
	rtn_code = RSAPrivateBlock (msg, msgLen, cipher, cipherLen, &cardRsaPrivateKey);
    if(rtn_code != 0){
        return FUNC_ERR;
    }
    
    return FUNC_OK;
	
}

CRET Crypt_Cert_PubKeyEnc (
	char *der_cert,
	int der_certSize,
	unsigned char *msg,
	int msgLen,
	unsigned char *cipher,
	int *cipherLen)
{
    unsigned char subjectPublicKeyInfo[512];
    int  subjectPublicKeyInfoSize;
    RSA_PUBLIC_KEY  rsaPublicKey;

    int rtn_code;


    rtn_code = Crypt_Get_PubKey_FromCert(
                    der_cert,
                    der_certSize,
                    subjectPublicKeyInfo,
                    &subjectPublicKeyInfoSize);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

    rtn_code = d2c_RSA_PUBLIC_KEY(
                   subjectPublicKeyInfo,
                   subjectPublicKeyInfoSize,
                   &rsaPublicKey);
    if(rtn_code != FUNC_OK){
        return FUNC_ERR;
    }

	/*
    rtn_code = CSP_RSAPublicEncrypt(
		           0, 
                   cipher,
                   &cipherLen,
                   msg,
                   msgLen,
                   &rsaPublicKey);
	*/
	 rtn_code = 	RSAPublicBlock (cipher, cipherLen, msg, msgLen, &rsaPublicKey);
    if(rtn_code != 0){
        return FUNC_ERR;
    }

	return 0;
}

char *del_rn(char *pbuf)
{
  char *p;

  if((p =strchr(pbuf, '\r')) !=NULL) *p =0;
  if((p =strchr(pbuf, '\n')) !=NULL) *p =0;

  return pbuf;
}
