#pragma once
#include "main.h"

#define GInput_offset  0xB8
#define pdwLevel_offset  0xF
#define m_IAccount_offset 0x1448

#ifndef __OFFSETS__
#define __OFFSETS__

class poffsets
{
public:
	 DWORD dw_m_fFlags();

	 bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	 DWORD dwFindPattern(DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask);
};
extern poffsets m_offsets;

#endif