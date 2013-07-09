#include "decrypt_aes.h"

void DecryptAES::DecryptData(BYTE *pbuf, int len)
{
    AES_KEY decrypt_key;
    BYTE iv[16];
    memmove(iv, AES_cbc128, 16);

    AES_set_decrypt_key((BYTE*)AES_key128, AES_KEY_SIZE*8, &decrypt_key);
    AES_cbc_encrypt(pbuf, pbuf, (len/16)*16, &decrypt_key, iv, AES_DECRYPT);
}
