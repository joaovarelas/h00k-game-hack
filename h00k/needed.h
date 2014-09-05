#pragma once
#include "main.h"

class CNeeded
{
public:
void FontInit(void);
void FillRGB( int x, int y, int w, int h, DWORD colour ); //dwColor
void FillRGBA( int x, int y, int w, int h, int r, int g,int b, int a ); //rgba
bool __fastcall	GetVisible( Vector& vecAbsStart, Vector& vecAbsEnd, C_BaseEntity* pBaseEnt );
void DrawString( int x, int y, int r, int g,int b,int a, bool bCenter, const char *pszText, ... ); //with alpha
void DrawString2( vgui::HFont font, int x, int y, int r, int g, int b, int a, wchar_t *pszString, ... );
void blackBorder(int x,int y,int w,int h);
void Normalize(Vector &vIn, Vector &vOut);
bool WorldToScreen( const Vector &vOrigin, Vector &vScreen );
bool ScreenTransform( const Vector &point, Vector &screen );
void GetWorldSpaceCenter( CBaseEntity* pBaseEnt, Vector& vWorldSpaceCenter );
CBaseEntity* GetEntityByIndex( int idx );
C_BaseEntity *GetBaseEntityByIndex( int iIndex );
Vector &GetEyePosition(C_BaseEntity* pEntity,CUserCmd * c);
Vector &GetEyePositionNormal(C_BaseEntity* pEntity);
bool TransformVector( Vector v, Vector &s );
int getHeight(HFont);
int getWidth(const char *input,HFont);
typedef INetChannelInfo* (*InNet) (void);
InNet cNetc;
bool __fastcall bIsDefaultHit( trace_t *pTrace, C_BaseEntity *pEnt );
bool __fastcall bCheckVisible( Vector &vecAbs ,Vector &vecDest, C_BaseEntity *pEnt );
void GetTraceFilterSkipTwoEntities( void *first, void *second, DWORD* thisptr );
bool __fastcall IsAlive(C_BaseEntity* pEnt);

public:
	vgui::HFont font;
};

extern CNeeded gNeeded;



///////////////////////////////////////

class GUI
{
public:
	int AddMenuEntry(int n, wchar_t *title, float* value, float min, float max, float step);
	void Init();

	bool bActive;
	void MenuDraw();
	int  MenuNavigate(int keynum);
	int  MenuIndex;
};
extern GUI gui;


