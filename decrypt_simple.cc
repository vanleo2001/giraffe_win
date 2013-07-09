#include "decrypt_simple.h"

DWORD dwSimpleMima[] = {0xC5369AB5,0xA75AEC75,0x9ABE638A,0x7ACD89AF};

void DecryptSimple::DecryptData(BYTE *pBuf,int nLen)
{
 	int i;
	DWORD dwMask = 0x7B8934A8;
	for(i=0;i<(nLen%4);i++)
		dwMask = (dwMask^dwSimpleMima[i]) + dwMask*56909871 + (dwMask<<7);
	for(i=0;i<nLen-3;i+=4)
	{
		DWORD* pdw = (DWORD*)(pBuf+i);
		*pdw ^= dwMask;
		dwMask = (dwMask^dwSimpleMima[i%4]) + (dwMask<<5);
	}
	for(;i<nLen;i++)
	{
		pBuf[i] ^= dwMask;
		dwMask >>= 8;
	}
}
