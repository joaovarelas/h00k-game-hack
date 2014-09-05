#pragma once

#include <windows.h>
#include <process.h> 
#include <assert.h>
#include <tlhelp32.h>
#include <winsock.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <vector>
#include <fstream>
#include <string.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <const.h>
#include <WindowsX.h>
#include "detours.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "detours.lib") 

#define SECURITY_WIN32
#define WIN32_LEAN_AND_MEAN

#pragma optimize("gsy",on)

#include <Security.h>
#include <shlwapi.h>

#ifndef CLIENT_DLL
#define CLIENT_DLL

///////////
////SDK////
///////////

// First
#include "SDK\\public\\tier0\\icommandline.h"
#include "SDK\\public\\tier0\\wchartypes.h"

// Public
#include "SDK\\public\\cdll_int.h"
#include "SDK\\public\\mathlib\halton.h"
#include "SDK\\public\\mathlib\ssemath.h"
#include "SDK\\public\\iprediction.h"
#include "SDK\\public\\bone_setup.h"
#include "SDK\\public\\icliententitylist.h"
#include "SDK\\public\\ienginevgui.h"
#include "SDK\\public\\IGameUIFuncs.h"
#include "SDK\\public\\dlight.h"
#include "SDK\\public\\iefx.h" 
#include "SDK\\public\\igameevents.h"
#include "SDK\\public\\view_shared.h"
#include "SDK\\public\\inetchannel.h"
#include "SDK\\public\\iachievementmgr.h"
#include "SDK\\public\\steam\\steam_api.h"
#include "SDK\\public\\steam\\isteamuserstats.h"
#include "SDK\\public\\eiface.h"

// Client
#include "SDK\\game\\client\\imessagechars.h"
#include "SDK\\game\\client\\iclientmode.h"
#include "SDK\\game\\client\\cliententitylist.h"
#include "SDK\\game\\client\\cdll_client_int.h"
#include "SDK\\game\\client\\cbase.h"
#include "SDK\\game\\client\\c_baseanimating.h"
#include "SDK\\game\\client\\c_basecombatweapon.h"
#include "SDK\\game\\client\\c_baseplayer.h"
#include "SDK\\game\\client\\enginesprite.h"
#include "SDK\\game\\client\\input.h"
#include "SDK\\game\\client\\c_playerresource.h"
#include "SDK\\game\\client\\iviewrender.h"
#include "SDK\\game\\client\\viewrender.h"
#include "SDK\\game\\client\\game_controls\\commandmenu.h"
#include "SDK\\game\\client\\hudelement.h"
#include "SDK\\game\\client\\prediction.h"

// Server
//#include "SDK\\game\\server\\bg2\\weapon_bg2base.h"

// Engine
#include "SDK\\public\\engine\\ivmodelrender.h"
#include "SDK\\public\\engine\\ivdebugoverlay.h"
#include "SDK\\public\\engine\\ivmodelinfo.h"
#include "SDK\\public\\engine\\IEngineTrace.h"
#include "SDK\\public\\engine\\IEngineSound.h"

// Material System
#include "SDK\\public\\materialsystem\\imaterialsystemstub.h"
#include "SDK\\public\\materialsystem\\itexture.h"
#include "SDK\\public\\materialsystem\\IMaterialVar.h"

// VGUI
#include "SDK\\public\\matsys_controls\\matsyscontrols.h"
#include "SDK\\public\\vgui\\IClientPanel.h"
#include "SDK\\public\\vgui\\IPanel.h"
#include "SDK\\public\\vgui\\ISurface.h"

// VGUI Controls
#include "SDK\\public\\vgui\\ILocalize.h"
#include "SDK\\public\\collisionutils.h"

// VGUI Material Surface
#include "SDK\\public\\vgui_controls\\Panel.h"
#include "SDK\\public\\VGuiMatSurface\\IMatSystemSurface.h"

// Shared
#include "SDK\\game\\shared\\usermessages.h"
#include "SDK\\game\\shared\\basecombatweapon_shared.h"
#include "SDK\\game\\shared\\takedamageinfo.h"
#include "SDK\\game\\shared\\predicted_viewmodel.h"
#include "SDK\\game\\shared\\ipredictionsystem.h"
#include "SDK\\game\\shared\\predictableid.h"
#include "SDK\\game\\shared\\predictioncopy.h"
#include "SDK\\game\\shared\\base_playeranimstate.h"
#include "SDK\\game\\shared\\predictable_entity.h"
#include "SDK\\game\\shared\\igamemovement.h"
#include "SDK\\game\\shared\\prediction.h"
#include "SDK\\game\\shared\\ammodef.h"

// Others
#include "SDK\\game\\client\\c_vehicle_jeep.h"
#include "SDK\\game\\client\\prediction.h"
#include "SDK\\game\\client\\detailobjectsystem.h"
#include "SDK\\game\\server\\player_command.h"

#pragma comment(lib, "SDK/lib/public/tier0.lib")
#pragma comment(lib, "SDK/lib/public/tier1.lib")
#pragma comment(lib, "SDK/lib/public/tier2.lib")
#pragma comment(lib, "SDK/lib/public/tier3.lib")
#pragma comment(lib, "SDK/lib/public/mathlib.lib")
#pragma comment(lib, "SDK/lib/public/vstdlib.lib")
#pragma comment(lib, "SDK/lib/public/vtf.lib")
#pragma comment(lib, "SDK/lib/public/particles.lib")
#pragma comment(lib, "SDK/lib/public/matsys_controls.lib")
#pragma comment(lib, "SDK/lib/public/vgui_controls.lib")
#pragma comment(lib, "SDK/lib/public/bitmap.lib")
#pragma comment(lib, "SDK/lib/public/vmpi.lib")
#pragma comment(lib, "SDK/lib/public/raytrace.lib")
#pragma comment(lib, "SDK/lib/public/dmxloader.lib")
#pragma comment(lib, "SDK/lib/public/nvtristrip.lib")
#pragma comment(lib, "WINMM.LIB")

//////////////////
//HACK INCLUDES!//
//////////////////
#include "esp.h"
#include "aimbot.h"
#include "nospread.h"
#include "needed.h"
#include "offsets.h"
#include "cvars.h"


///////////
//DEFINES//
///////////
#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)
#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)

#define WEAPON_NULL					0
#define WEAPON_AK47					1
#define WEAPON_AUG					2
#define WEAPON_AWP					3
#define WEAPON_DEAGLE				4
#define WEAPON_ELITES				5
#define WEAPON_FAMAS				6
#define WEAPON_FIVESEVEN			7
#define WEAPON_G3SG1				8
#define WEAPON_GALIL				9
#define WEAPON_GLOCK18				10
#define WEAPON_M249					11
#define WEAPON_M3					12
#define WEAPON_M4A1					13
#define WEAPON_MAC10				14
#define WEAPON_MP5					15
#define WEAPON_P228					16
#define WEAPON_P90					17
#define WEAPON_SCOUT				18
#define WEAPON_SG550				19  
#define WEAPON_SG552				20 
#define WEAPON_TMP					21
#define WEAPON_UMP45				22
#define WEAPON_USP45				23
#define WEAPON_XM1014				24
#define WEAPON_KNIFE				25
#define WEAPON_FLASHBANG			26 
#define WEAPON_HE_GRENADE			27
#define WEAPON_SMOKE_GRENADE		28 
#define WEAPON_C4					29

#define COLORCODE(r,g,b,a)			((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))
#define RED(COLORCODE)				((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)				((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)			((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)			((int) COLORCODE & 0xFF )
#define RGBA(COLORCODE)				RED( COLORCODE ), GREEN( COLORCODE ), BLUE( COLORCODE ), ALPHA( COLORCODE )

#define MENU_BG COLORCODE( 0, 0, 0, 120 )//background color behind menu items
#define MENU_TEXT COLORCODE( 0, 150, 255, 250 )//menu text color
#define MENU_SELECTION COLORCODE( 0, 0, 0, 255 )//top half of selection bar color
#define MENU_SELECTIONB COLORCODE( 0, 0, 0, 255 )//bottom half of slection bar color
#define MENU_TITLE COLORCODE( 171, 242, 4, 255 )//color of text in title
#define MENU_TITLEBG COLORCODE( 253, 77, 2, 120 )//top half of title bar background color
#define MENU_TITLEBGB COLORCODE( 254, 124, 69, 120 )//bottom  half of title bar background color

/////////
//MATHS//
/////////
#define M_RADPI 57.295779513082f
#define SQUARE( a ) a*a

static const float PI = 3.14159265358979323846f;
static const float PIdiv = 0.01745329251994329576f;
static const float divPI = 57.29577951308232087684f;

//////////////
//SET EXTERN//
//////////////
extern IBaseClientDLL*	          g_pClient;
extern CInput*			          g_pInput;
extern IVEngineClient*	          g_pEngine;
extern IClientEntityList*         g_pEntList;
extern IEngineTrace*	          g_pEnginetrace;
extern IVModelInfoClient*         g_pModelinfo;
extern IVPhysicsDebugOverlay*     g_pDebugOverlay;
extern ICvar*                     g_pCvar;
extern IPanel*					  g_pPanel;
extern IMaterialSystem*			  g_pMatSystem;
extern IVRenderView*			  g_pRenderView;
extern IGameEventManager2*	      g_pEventManager2;
extern ISurface*				  g_pSurface;
extern IMatSystemSurface*         g_pMatySystemSurface;
extern ILocalize*			      g_pVGUILocalize;
extern IPrediction*				  g_pPrediction;
extern CGlobalVarsBase*           g_pGlobals;
extern IPhysicsSurfaceProps*      g_pPhysicAPI;
extern IVEfx*                     g_pEffects;
extern IVModelRender*			  g_pModelRender;
extern INetChannelInfo*			  g_pNetworkStatus;
extern HFont                      font;
extern HFont                      font2;

////////////
//dwEXTERN//
////////////
extern DWORD g_dwOrgInit;
int __stdcall new_Init( CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory,CGlobalVarsBase *pGlobals );

extern DWORD g_dwOrgCreateMove;
void __stdcall new_CreateMove(int sequence_number, float input_sample_frametime, bool active);
typedef void (WINAPI* tCreateMove)(int,float,bool);
extern tCreateMove pCreateMove;

extern DWORD g_dwOrgEngineSBegin;
void __stdcall new_EngineStats_BeginFrame( void );

extern DWORD g_dwOrgHudUpdate;
void __stdcall new_HudUpdate( bool bActive );

extern DWORD g_dwUserDecodeCmd;
void __stdcall new_DecodeUserCmdFromBuffer( bf_read& buf, int sequence_number );

extern DWORD g_dwUserEncodeCmd;
void __stdcall new_EncodeUserCmdToBuffer( bf_write& buf, int sequence_number );

extern DWORD g_dwUserWirtCmd;
bool __stdcall new_WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand );

extern DWORD g_dwOrgPaintTraverse;
void __stdcall new_PaintTraverse(vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce);

typedef void (WINAPI* tPaintTraverse)(vgui::VPANEL, bool, bool);
extern tPaintTraverse pPaintTraverse;

extern DWORD g_OrgDrawModelExecute;
void __stdcall new_DrawModelExecute(ModelRenderInfo_t &pInfo);

extern DWORD g_dwOrgFindMaterial;
IMaterial* __stdcall new_FindMaterial( char const* pMaterialName, const char *pTextureGroupName, bool complain, const char *pComplainPrefix );

void __stdcall new_GetViewAngles(QAngle &va);

extern DWORD g_dwOrgSetViewAngles;
void __stdcall new_SetViewAngles(QAngle &va);

extern DWORD g_dwDecodeUserCmdFromBuffer;
void __stdcall new_DecodeUserCmdFromBuffer( bf_read& buf, int sequence_number );

extern DWORD g_dwOrgGetUserCmd;
CUserCmd* __stdcall new_GetUserCmd( int sequence_number );
extern DWORD dwTraceFilter,dwGetTextureInformation,dwGetBulletTypeParameters;
extern DWORD dwFindPattern(DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask);


#define GetPattern(_DWORD_,module,\
	size,sig,mask,add_byte)    static DWORD _DWORD_ = NULL; if(_DWORD_ == NULL){_DWORD_ = m_offsets.dwFindPattern((DWORD)GetModuleHandleA(module),size,(PBYTE)sig,mask) + add_byte;_DWORD_ = *(PDWORD)_DWORD_;} return _DWORD_;  
#define GetPattern_PTR(_DWORD_PTR_,_DWORD_,module,size,sig,mask,add_byte)    static DWORD_PTR _DWORD_PTR_ = NULL; static DWORD _DWORD_ = NULL; if(_DWORD_ == NULL) { _DWORD_ = m_offsets.dwFindPattern((DWORD)GetModuleHandleA(module),size,(PBYTE)sig,mask) + add_byte; _DWORD_PTR_ = DWORD_PTR( *PDWORD_PTR( _DWORD_ )); _DWORD_PTR_ = DWORD_PTR( *PDWORD_PTR( _DWORD_PTR_ )); } return DWORD_PTR(_DWORD_PTR_);
#define CT ( (NumVertices == 3417 && primCount == 5030) || (NumVertices == 2245 && primCount == 2998) || (NumVertices == 1609 && primCount == 1794) || (NumVertices == 929 && primCount == 1007) || (NumVertices == 499 && primCount == 533) || (NumVertices == 409 && primCount == 433) || (NumVertices == 3887 && primCount == 4974) || (NumVertices == 2487 && primCount == 3006) || (NumVertices == 2130 && primCount == 3004) || (NumVertices == 899 && primCount == 910) || (NumVertices == 513 && primCount == 451) || (NumVertices == 369 && primCount == 297) ||  (NumVertices == 3206 && primCount == 4872) || (NumVertices == 1498 && primCount == 1822) || (NumVertices == 1424 && primCount == 1858) || (NumVertices == 814 && primCount == 945)||(NumVertices == 475 && primCount == 510) || (NumVertices == 343 && primCount == 344) || (NumVertices == 3304 && primCount == 5003) || (NumVertices == 2196 && primCount == 3061)|| (NumVertices == 1404 && primCount == 1919) || (NumVertices == 811 && primCount == 989) || (NumVertices == 466 && primCount == 550) || (NumVertices == 324 && primCount == 372) )
#define T ( (NumVertices == 3561 && primCount == 5066) || (NumVertices == 2261 && primCount == 3003) || (NumVertices == 1588 && primCount == 2013) || (NumVertices == 911 && primCount == 996) || (NumVertices == 526 && primCount == 594) || (NumVertices == 389 && primCount == 386) || (NumVertices == 3265 && primCount == 5015) || (NumVertices == 2274 && primCount == 3070) || (NumVertices == 1510 && primCount == 1871) || (NumVertices == 873 && primCount == 986) || (NumVertices == 563 && primCount == 590) || (NumVertices == 368 && primCount == 377) || (NumVertices == 3087 && primCount == 4911) || (NumVertices == 1992 && primCount == 2996) || (NumVertices == 1311 && primCount == 1812) || (NumVertices == 735 && primCount == 899) || (NumVertices == 454 && primCount == 519) || (NumVertices == 323 && primCount == 338) || (NumVertices == 3210 && primCount == 4503) || (NumVertices == 2292 && primCount == 3015) || (NumVertices == 1488 && primCount == 1819) || (NumVertices == 835 && primCount == 899) || (NumVertices == 554 && primCount == 509) || (NumVertices == 408 && primCount == 344) )
#define HANDS (NumVertices == 1286 && primCount == 1778)
#define SKY (g_cBase.m_nStride == 32 && NumVertices == 4 && primCount == 2 && ((startIndex == 0 || startIndex == 6 || startIndex == 12 || startIndex == 18 || startIndex == 24 /*|| startIndex == 54 || startIndex == 60 || startIndex == 66 || startIndex == 72*/ ) && (MinVertexIndex == 0 || MinVertexIndex == 4 || MinVertexIndex == 8 || MinVertexIndex == 12)))
#define WEAPON ( (NumVertices == 403 && primCount == 269) || (NumVertices == 499 && primCount == 326) || (NumVertices == 336 && primCount == 220) || (NumVertices == 394 && primCount == 263) || (NumVertices == 399 && primCount == 271) || (NumVertices == 724 && primCount == 496) || (NumVertices == 469 && primCount == 329) || (NumVertices == 456 && primCount == 382) || (NumVertices == 450 && primCount == 323) || (NumVertices == 394 && primCount == 280) || (NumVertices == 502 && primCount == 410) || (NumVertices == 560 && primCount == 391) || (NumVertices == 465 && primCount == 329) || (NumVertices == 431 && primCount == 285) || (NumVertices == 758 && primCount == 462) || (NumVertices == 462 && primCount == 364) || (NumVertices == 606 && primCount == 477) ||(NumVertices == 616 && primCount == 494) ||(NumVertices == 620 && primCount == 464) || (NumVertices == 628 && primCount == 402) || (NumVertices == 681 && primCount == 523) || (NumVertices == 376 && primCount == 300) ||(NumVertices == 581 && primCount == 433) || (NumVertices == 671 && primCount == 489) || (NumVertices == 753 && primCount == 541) || (NumVertices == 650 && primCount == 522) || (NumVertices == 573 && primCount == 417) || (NumVertices == 671 && primCount == 493) )
#define COLORCODE(r,g,b,a)((DWORD)((((r)&0xff)<<24)|(((g)&0xff)<<16)|(((b)&0xff)<<8)|((a)&0xff)))
#define RED(COLORCODE)((int) ( COLORCODE >> 24) )
#define BLUE(COLORCODE)((int) ( COLORCODE >> 8 ) & 0xFF )
#define GREEN(COLORCODE)((int) ( COLORCODE >> 16 ) & 0xFF )
#define ALPHA(COLORCODE)((int) COLORCODE & 0xFF )
#define RGBA(COLORCODE)RED( COLORCODE ), GREEN( COLORCODE ), BLUE( COLORCODE ), ALPHA( COLORCODE )

#define DO_ONCE( arg )				\
	static bool UnIqUe_ONC3 = false;\
	if( UnIqUe_ONC3 == false )\
{									\
	UnIqUe_ONC3 = true;				\
	arg								\
}

#define CHEAT_ORANGE COLORCODE( 255, 100, 0, 255 )
#define CHEAT_YELLOW COLORCODE( 255, 255, 0, 255 )
#define CHEAT_PURPLE COLORCODE( 55, 25, 128, 255 )
#define CHEAT_RED COLORCODE( 255, 0, 0, 255 )
#define CHEAT_GREEN COLORCODE( 0, 255, 0, 255 )
#define CHEAT_BLUE COLORCODE( 0, 0, 255, 255 )
#define CHEAT_BLACK COLORCODE( 0, 0, 0, 255 )
#define CHEAT_WHITE COLORCODE( 255, 255, 255, 255 )
#define CHEAT_PINK COLORCODE( 255, 0, 255, 255 )
#define CHEAT_LIGHTBLUE COLORCODE( 0, 0, 225, 225 )
#define CHEAT_LIGHTGREEN COLORCODE( 0, 225, 0, 225 )
#define CHEAT_LIGHTRED COLORCODE( 225, 0, 0, 225 )

#define KEYDOWN(vk_code)((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)



#pragma warning( disable:4409 )
#pragma warning( disable:4172 )


#endif