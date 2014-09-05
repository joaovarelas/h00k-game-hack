#include "main.h"
#include <map>
#ifndef _AIMBOT_H_
#define _AIMBOT_H_
#define CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_BLOODYFLESH	'B'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_PLASTIC		'L'

class cAimbot
{
public:

	cAimbot();

	int nTarget;
	float	flBestDist;
	void __fastcall Bot(CUserCmd* c);
	bool __fastcall	GetVisible( Vector& vecAbsStart, Vector& vecAbsEnd, C_BaseEntity* pBaseEnt );
	void __fastcall BotHitscan(CUserCmd* c);
	void __fastcall CalculateAngle(Vector &vSource, Vector &vDestination, QAngle &qAngle);
	C_BaseEntity * __fastcall GetFirstEntity(C_BaseEntity *pMe);
	C_BaseEntity* __fastcall GetNearest(C_BaseEntity *pMe, Vector vMyEyePosition);
	bool __fastcall HasAmmo(C_BaseEntity *pEnt);
	bool __fastcall IsAlive(C_BaseEntity* pEnt);
	void	MakeVector( QAngle angle, QAngle& vector );
	void  __fastcall CalcAngle( Vector &src, Vector &dst, QAngle &angles );
	float	GetFov( QAngle angle, Vector src, Vector dst );
	bool __fastcall bHasMiscWeapon();
	Vector m_vecPosition;
	Vector GetVelocity(C_BaseEntity* pEnt);
	void __fastcall Reset();
	Vector vEyeEnemyPos;
	Vector vEyePos;
	Vector m_vecFinal;
	static C_BaseCombatWeapon* GetBaseCombatActiveWeapon( C_BaseEntity* pBaseEntity );
	static int getWeaponID(const char *weaponName);
};
extern cAimbot g_pAimbot;

#endif