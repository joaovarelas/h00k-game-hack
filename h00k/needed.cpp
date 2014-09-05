///////////////
//NEEDEDFUNCS//
///////////////

#include "needed.h"
#include <sstream>

CNeeded gNeeded;



void CNeeded::FontInit()
{
	font = g_pSurface->CreateFont();
		g_pSurface->SetFontGlyphSet(font,"Arial",12,400,0,0,0x200);
}

void CNeeded::DrawString( int x, int y, int r, int g,int b, int a, bool bCenter, const char *pszText, ... )
{
	if( pszText == NULL )
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start( va_alist, pszText );
	vsprintf( szBuffer, pszText, va_alist );
	va_end( va_alist );
	DWORD color = COLORCODE(r,g,b,a);

	wsprintfW( szString, L"%S", szBuffer );
	int iWidth, iHeight;
	g_pMatySystemSurface->GetTextSize( font, szString , iWidth, iHeight );
	g_pSurface->DrawSetTextFont(font);
	g_pMatySystemSurface->DrawSetTextPos( x - ( bCenter ? iWidth / 2 : 0 ), y );
	g_pSurface->DrawSetTextColor( Color(RED(color),GREEN(color),BLUE(color),ALPHA(color)) );
	g_pSurface->DrawPrintText( szString, wcslen( szString ) );
}

void CNeeded::DrawString2( vgui::HFont font, int x, int y, int r, int g, int b, int a, wchar_t *pszString, ... )
{
	g_pSurface->DrawSetTextColor( r, g, b, a );
	g_pSurface->DrawSetTextFont( font );
	g_pSurface->DrawSetTextPos( x, y );
	g_pSurface->DrawPrintText( pszString, ( int )wcslen( pszString ), FONT_DRAW_DEFAULT );
}

bool CNeeded::ScreenTransform( const Vector &point, Vector &screen )
 {
	float w;
	const VMatrix &worldToScreen = g_pEngine->WorldToScreenMatrix();
	screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	w		 = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	screen.z = 0.0f;
	bool behind = false;
	if( w < 0.001f ){
		behind = true;
		screen.x *= 100000;
		screen.y *= 100000;
	}
	else{
		behind = false;
		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
	}
	return behind;
}

bool CNeeded::WorldToScreen( const Vector &vOrigin, Vector &vScreen )
 {
	if( ScreenTransform(vOrigin , vScreen) == false ){
		int iScreenWidth, iScreenHeight;
		g_pEngine->GetScreenSize( iScreenWidth, iScreenHeight );
		float x = iScreenWidth / 2;
		float y = iScreenHeight / 2;
		x += 0.5 * vScreen.x * iScreenWidth + 0.5;
		y -= 0.5 * vScreen.y * iScreenHeight + 0.5;
		vScreen.x = x;
		vScreen.y = y;
		return true;
	}
	return false;
} 

void CNeeded::Normalize(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();

	if(flLen == 0)
	{
		vOut.Init(0, 0, 1);
		return;
	}

	flLen = 1 / flLen;

	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}

void CNeeded::FillRGB( int x, int y, int w, int h, DWORD colour )
{
	g_pMatySystemSurface->DrawSetColor( RED(colour), GREEN(colour), BLUE(colour), ALPHA(colour) );
	g_pMatySystemSurface->DrawTexturedRect( x, y, x + w, y + h );
}

void CNeeded::FillRGBA( int x, int y, int w, int h, int r, int g,int b, int a )
{
	g_pMatySystemSurface->DrawSetColor(r, g, b, a );
	g_pMatySystemSurface->DrawFilledRect( x, y, x + w, y + h );
}

void CNeeded::blackBorder(int x,int y,int w,int h)
{
	gNeeded.FillRGBA(x-1,y-1,w+2,1,0,0,0,255); //Top
	gNeeded.FillRGBA(x-1,y,1,h-1,0,0,0,255);	//Left
	gNeeded.FillRGBA(x+w,y,1,h-1,0,0,0,255);	//Right
	gNeeded.FillRGBA(x-1,y+h-1,w+2,1,0,0,0,255); //Bottom
}

C_BaseEntity *CNeeded::GetBaseEntityByIndex( int iIndex )
{
	IClientEntity *pClientEnt = g_pEntList->GetClientEntity( iIndex );
	if( !pClientEnt ) { return NULL; }
	return pClientEnt->GetBaseEntity();
}

CBaseEntity* CNeeded::GetEntityByIndex( int idx )
{
	if( g_pEntList == NULL ) return NULL;
	IClientEntity *pClient = g_pEntList->GetClientEntity( idx );
	if( pClient == NULL ) return NULL;
	return pClient->GetBaseEntity();
}

void CNeeded::GetWorldSpaceCenter( CBaseEntity* pBaseEnt, Vector& vWorldSpaceCenter )
{
	if ( pBaseEnt )
	{
		Vector vMin, vMax;
		pBaseEnt->GetRenderBounds( vMin, vMax );
		vWorldSpaceCenter = pBaseEnt->GetAbsOrigin();
	}
}

Vector &CNeeded::GetEyePosition(C_BaseEntity* pEntity,CUserCmd * c)
{
	Vector m_vecOrigin = pEntity->GetAbsOrigin();
    Vector m_vecViewOffset;
	m_vecViewOffset[0] = *(float*)((DWORD)pEntity + 0x0E4);
	m_vecViewOffset[1] = *(float*)((DWORD)pEntity + 0x0E8);
	m_vecViewOffset[2] = *(float*)((DWORD)pEntity + 0x0EC);
	Vector vEyePosition = (m_vecOrigin + m_vecViewOffset);
	return vEyePosition;
}

Vector &CNeeded::GetEyePositionNormal(C_BaseEntity* pEntity)
{
    Vector m_vecOrigin = pEntity->GetAbsOrigin(); //*(Vector*)((DWORD)pEntity + 0x334); //2FC
    Vector m_vecViewOffset;
	m_vecViewOffset.x = *(float*)((DWORD)pEntity + 0x0E4);
	m_vecViewOffset.y = *(float*)((DWORD)pEntity + 0x0E8);
	m_vecViewOffset.z = *(float*)((DWORD)pEntity + 0x0EC);
	Vector vEyePosition = (m_vecOrigin + m_vecViewOffset);
	return vEyePosition;
}

bool CNeeded::TransformVector( Vector v, Vector &s )
{
	if( ScreenTransform( v, s ) == false ){
		int sw, sh;
		g_pEngine->GetScreenSize( sw, sh );
		float x = sw / 2;
		float y = sh / 2;
		x += 0.5 * s.x * sw + 0.5;
		y -= 0.5 * s.y * sh + 0.5;
		s.x = x;
		s.y = y;
		return true;
	}
	return false;
}

int CNeeded::getWidth(const char *input,HFont espFont){
	return g_pMatySystemSurface->DrawTextLen(espFont,(char*)input);
}

int CNeeded::getHeight(HFont espFont){
	return g_pMatySystemSurface->GetFontTall(espFont);
}

bool __fastcall CNeeded::bIsDefaultHit( trace_t *pTrace, C_BaseEntity *pEnt )
{
    if( !pEnt )
        return false;

    if( pTrace->allsolid || pTrace->startsolid )
        return false;
        
    if( pTrace->m_pEnt && ( pTrace->m_pEnt->index == pEnt->index ) )
        return true;

    if( pTrace->fraction >= 1.0f )
        return true;

    return false;
}

void CNeeded::GetTraceFilterSkipTwoEntities( void *first, void *second, DWORD* thisptr )
{
	typedef void ( __thiscall *TraceFilterSkipTwoEntities_t )( DWORD *thisptr, void *first, void *second, int group );
	static TraceFilterSkipTwoEntities_t traceFilterskiptwoentities = (TraceFilterSkipTwoEntities_t)((DWORD)GetModuleHandleA( "client.dll" )+0x1AA7F0);
	traceFilterskiptwoentities( thisptr, first, second, 0);
}

bool __fastcall CNeeded::GetVisible( Vector& vecAbsStart, Vector& vecAbsEnd, C_BaseEntity* pBaseEnt ) 
{ 
	player_info_t pinfo;
	trace_t tr;
	Ray_t ray;

	ray.Init( vecAbsStart, vecAbsEnd );
	g_pEnginetrace->TraceRay( ray, MASK_NPCWORLDSTATIC|CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX, NULL, &tr );

	if ( tr.fraction > 0.97f )
		return true;


	/*if (gCvar.aim_autowall)
	{
		surfacedata_t *pSurfaceData = Jorg.m_pPhysicAPI->GetSurfaceData( tr.surface.surfaceProps );
		int iMaterial = pSurfaceData->game.material;

		switch ( iMaterial )
		{
		case CHAR_TEX_METAL:
			return false;	// even an awm can't do much now...
		case CHAR_TEX_CONCRETE:
			return false;	// even an awm can't do much now...
		case CHAR_TEX_DIRT:
			return false;	// even an awm can't do much now...
		case CHAR_TEX_SLOSH:
			return false;	// even an awm can't do much now...
		case CHAR_TEX_SAND:
			return false;	// even an awm can't do much now...
		case CHAR_TEX_GRATE:
			return true;
		case CHAR_TEX_VENT:
			return true;
		case CHAR_TEX_TILE:
			return true;
		case CHAR_TEX_COMPUTER:
			return true;
		case CHAR_TEX_WOOD:
			return true;
		case CHAR_TEX_ANTLION:
			return true;
		case CHAR_TEX_CLIP:
			return true;
		case CHAR_TEX_ALIENFLESH:
			return true;
		case CHAR_TEX_FOLIAGE:
			return true;
		case CHAR_TEX_GLASS:
			return true;
		case CHAR_TEX_PLASTIC:
			return true;
		case CHAR_TEX_BLOODYFLESH:
			return true;
		case CHAR_TEX_FLESH:
			return true;
		default :
			return false;
		}*/

		if ( tr.fraction != 1.0 )
		{
			if (  tr.allsolid /*|| tr.DidHitWorld()*/)
				return false;
		}
		else
		{
			return true;
		}

	if ( tr.m_pEnt && pBaseEnt )
	{
		if ( tr.m_pEnt->index == 0 || tr.allsolid )
			return false;

		if (( g_pEngine->GetPlayerInfo( tr.m_pEnt->index, &pinfo )
			|| pBaseEnt->index == tr.m_pEnt->index) && tr.fraction > 0.92)
			return true;
	}
	return false;

}

bool __fastcall CNeeded::IsAlive(C_BaseEntity* pEnt)
{
	char lifestate = *(PCHAR)((DWORD)pEnt + 0x8F);
	if(!(lifestate == LIFE_ALIVE))
		return false;
	return true;
}





////////////
////MENU////
////////////
bool bKeyPressed[256];
bool IsKeyPressed(int iKey){
	//used for menu toggle
	if (GetAsyncKeyState(iKey)){
		if (!bKeyPressed[iKey]){
			bKeyPressed[iKey] = true;
			return true;
		}
	}
	else
		bKeyPressed[iKey] = false;

	return false;
}

struct menu_s
{
	wchar_t *title;
	float* value;
	float min;
	float max;
	float step;
};

int menuIndex = 0;int menuItems = 0;

menu_s menu[150];

GUI gui;

int GUI::AddMenuEntry(int n, wchar_t *title, float* value, float min, float max, float step)
{
	menu[n].title = title;
	menu[n].value = value;
	menu[n].min = min;
	menu[n].max = max;
	menu[n].step = step;
	return (n+1);
}

void GUI::Init()
{
	int i = 0;


	i = AddMenuEntry(i, L"Aimbot", &Cvars.menu1, 0, 1, 1);

	if(Cvars.menu1==1)
	{
		i = AddMenuEntry(i, L"  Aim Active", &Cvars.aim_bot, 0, 1, 1);
		i = AddMenuEntry(i, L"  Autoshoot", &Cvars.aim_shoot, 0, 1, 1);
		i = AddMenuEntry(i, L"  Aim Key", &Cvars.aim_key, 0, 1, 1);
		i = AddMenuEntry(i, L"  Aim Spot", &Cvars.aim_spot, 1, 12, 1);
		i = AddMenuEntry(i, L"  Aim Smooth", &Cvars.aim_smooth, 0, 50, 1);
		i = AddMenuEntry(i, L"  Misc Weapon", &Cvars.aim_miscweap, 0, 1, 1);
		i = AddMenuEntry(i, L"  FOV", &Cvars.aim_fov, 1, 360, 1);
		i = AddMenuEntry(i, L"  Silent Aim", &Cvars.aim_silent, 0, 1, 1);
	}

	i = AddMenuEntry(i, L"ESP", &Cvars.menu2, 0, 1, 1);

	if(Cvars.menu2==1)
	{
		i = AddMenuEntry(i, L"  Box ESP", &Cvars.esp_box, 0, 1, 1);
		i = AddMenuEntry(i, L"  Name ESP", &Cvars.esp_name, 0, 1, 1);
		i = AddMenuEntry(i, L"  Weapon ESP", &Cvars.esp_weapon, 0, 1, 1);
		i = AddMenuEntry(i, L"  Health ESP", &Cvars.esp_healthbar, 0, 1, 1);
		i = AddMenuEntry(i, L"  Visible ESP", &Cvars.esp_visible, 0, 1, 1);
	}

	i = AddMenuEntry(i, L"Removals", &Cvars.menu3, 0, 1, 1);

	if(Cvars.menu3==1)
	{
		i = AddMenuEntry(i, L"  No VisRecoil", &Cvars.rem_visrecoil, 0, 1, 1);
		i = AddMenuEntry(i, L"  No Recoil", &Cvars.rem_recoil, 0, 1, 1);
		i = AddMenuEntry(i, L"  No Spread", &Cvars.rem_spread, 0, 1, 1);
		i = AddMenuEntry(i, L"  No Flash", &Cvars.rem_flash, 0, 1, 1);
		i = AddMenuEntry(i, L"  No Smoke", &Cvars.rem_smoke, 0, 1, 1);
	}

	i = AddMenuEntry(i, L"Misc", &Cvars.menu4, 0, 1, 1);

	if(Cvars.menu4==1)
	{
		i = AddMenuEntry(i, L"  Anti-Aim", &Cvars.misc_antiaim, 0, 2, 1);
		i = AddMenuEntry(i, L"  Autopistol", &Cvars.misc_autopistol, 0, 1, 1);
		i = AddMenuEntry(i, L"  Bhop", &Cvars.misc_bhop, 0, 1, 1);
		//i = AddMenuEntry(i, L"  Speedhack", &Cvars.misc_speedhack, 0, 1, 1);
		i = AddMenuEntry(i, L"  Spinbot", &Cvars.misc_spinbot, 0, 1, 1);
		i = AddMenuEntry(i, L"  Light Spam", &Cvars.misc_lightspam, 0, 1, 1);
		i = AddMenuEntry(i, L"  Watermark", &Cvars.misc_watermark, 0, 1, 1);
	}

	i = AddMenuEntry(i, L"Menu POS", &Cvars.menu5, 0, 1, 1);

	if(Cvars.menu5==1)
	{
	i = AddMenuEntry(i, L"  Menu X", &Cvars.menu_x, 1, 600, 1);
	i = AddMenuEntry(i, L"  Menu Y", &Cvars.menu_y, 1, 600, 1);
	}

	menuItems=i;
}


void GUI::MenuDraw()
{

	/////////////
	//WATERMARK//
	/////////////
	if(Cvars.misc_watermark==1)
	{
	gNeeded.DrawString2((HFont)6,55,5,255,255,255,255,L"h00k");
	}

	if (IsKeyPressed(VK_INSERT))
    {
		bActive = !bActive;
    }

	if (!bActive) return;


	int x = Cvars.menu_x;
	int y = Cvars.menu_y;
	int h = 16;
	int w = 130;

	gNeeded.FillRGBA(x,y,w,h,240,150,46,170); //TITLE BOX
	gNeeded.blackBorder(x,y,w,h);
	gNeeded.DrawString2((HFont)10,x + 15,y,255,255,255,255,L"h00k by johnny");

	gNeeded.FillRGBA(x,y + h + 5,w,menuItems * 16,130,130,130,170); //MENU BOX
	gNeeded.blackBorder(x,y + h + 5,w,menuItems * 16);


	for(int i=0;i<menuItems;i++)
	{

		int vIn = (int)*menu[i].value;
		wchar_t vOut [12];
		_itow_s(vIn,vOut,sizeof(vOut)/2,10);


		if( i!=menuIndex)
		{	
			gNeeded.DrawString2((HFont)10,x + 3, y + (16*i) + 20, 60, 60, 60,255,menu[i].title);
			gNeeded.DrawString2((HFont)10,x + 100, y + (16*i) + 20, 60,60,60,255,vOut);
		}
		else
		{
			gNeeded.DrawString2((HFont)10,x + 3, y + (16*i) + 20, 255, 255, 255,255,menu[i].title);
			gNeeded.DrawString2((HFont)10,x + 100, y + (16*i) + 20, 255,255,255,255,vOut);
		}
	}

	


		if(IsKeyPressed(VK_UP)) //uparrow || mwheelup
		{
		if( menuIndex>0 ) menuIndex--;
		else menuIndex = menuItems - 1;
		}

		else if( IsKeyPressed(VK_DOWN)) //downarrow || mwheeldown
		{
		if( menuIndex<menuItems-1 ) menuIndex++;
		else menuIndex = 0;
		}


		else if( IsKeyPressed(VK_LEFT) ) //leftarrow || leftbutton
		{
			if( menu[menuIndex].value )
			{
			menu[menuIndex].value[0] -= menu[menuIndex].step;
			if( menu[menuIndex].value[0] < menu[menuIndex].min )
				menu[menuIndex].value[0] = menu[menuIndex].max;
			}
		}

		else if( IsKeyPressed(VK_RIGHT) ) //rightarrow || rightbutton
		{
		if( menu[menuIndex].value )
		{
			menu[menuIndex].value[0] += menu[menuIndex].step;
			if( menu[menuIndex].value[0] > menu[menuIndex].max )
				menu[menuIndex].value[0] = menu[menuIndex].min;
		}
		
		}
		
}
