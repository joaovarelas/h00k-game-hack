#pragma once
#include "main.h"

class cNospread
{
public:
   static VOID GetSpreadFix(UINT seed,QAngle& vIn,QAngle& vOut );
   static C_BaseCombatWeapon* GetBaseCombatActiveWeapon( C_BaseEntity* pBaseEntity );
   static int getWeaponID(const char *weaponName);
};


extern cNospread gNospread;// OMFG 
