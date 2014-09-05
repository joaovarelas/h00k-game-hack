///////////
//OFFSETS//
///////////

#include "offsets.h"

poffsets m_offsets;
char* Modules[2] = { "client.dll" , "d3d9.dll" };
DWORD CLIENT_SIZE = 0x848000 , D3D9_SIZE = 0x1c3000;


bool poffsets::bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for(; *szMask; ++szMask, ++pData, ++bMask)
	{
		if(*szMask == 'x' && *pData != *bMask)
		{
			return false;
		}
	}
	return (*szMask) == NULL;
}

DWORD poffsets::dwFindPattern(DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask)
{
	for(DWORD i = NULL; i < dwSize; i++)
	{
		if(bDataCompare((BYTE*)(dwAddress + i), pbMask, szMask))
		{
			return (DWORD)(dwAddress + i);
		}
	}
	return 0;
}


DWORD poffsets::dw_m_fFlags()
{
	GetPattern(dw_m_fFlags,Modules[0],CLIENT_SIZE,"\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x30\x68\x00\x00\x00\x00\x6A\x07\x68\x00\x00\x00\x00\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xB8\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xB8\x00\x00\x00\x00","x????x????x????x????xxxx????xxx????x????x????x????xxxxxxxxxxxxxx????",0x1);
}
