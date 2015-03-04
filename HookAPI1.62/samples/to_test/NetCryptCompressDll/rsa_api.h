#pragma once

int rsa_gen_keypair(unsigned char *pub_key, int *pub_key_len, unsigned char *pri_key, int *pri_key_len)
{
}

int rsa_save_key(char *file_name, char *password, unsigned char *key, int key_len, unsigned char *pri_key, int pri_key_len)
{
}

int rsa_read_key(char *file_name, char *password, unsigned char *pub_key, int *pub_key_len, unsigned char *pri_key, int *pri_key_len)
{
}

int rsa_crypt_data(unsigned char *key, int key_len, unsigned char *data, int data_len, unsigned char *out_data, int *out_data_len)
{
}

int rsa_decrypt_data(unsigned char *key, int key_len, unsigned char *data, int data_len, unsigned char *out_data, int *out_data_len)
{
}
