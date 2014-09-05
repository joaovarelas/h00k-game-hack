///////////
//DLLMAIN//
///////////

#pragma optimize("", off)

#include "main.h"

//////////
//CLIENT//
//////////



/////////////
//SEEDS CMD//
/////////////
CUserCmd* __stdcall new_GetUserCmd( int sequence_number )
{
DWORD GetUserCmd = *( DWORD* )( ( DWORD ) g_pInput + 0xC4 ) + ( sequence_number  % 90 << 6 );
CUserCmd *pCmd = ( CUserCmd* ) GetUserCmd;

pCmd->random_seed = 165;
pCmd->command_number = 189;

if(Cvars.misc_lightspam==1)
{
pCmd->impulse = 100; //FLASHLIGHT SPAM
}


return pCmd;
}

//////////////
//CREATEMOVE//
//////////////

DWORD g_dwOrgCreateMove;
tCreateMove pCreateMove;

void __stdcall new_CreateMove(int sequence_number, float input_sample_frametime, bool active)
{
	g_pNetworkStatus = gNeeded.cNetc();
	pCreateMove(sequence_number,input_sample_frametime,active);
	DWORD_PTR m_dwEBP;
	__asm MOV m_dwEBP, EBP;
	bool * bSendPackets = ( bool *)( *( PDWORD )( m_dwEBP ) - 0x1 );

	C_BaseEntity*pBaseEntity = (C_BaseEntity*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );
	C_BasePlayer*pBasePlayer = (C_BasePlayer*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );
	CInput::CVerifiedUserCmd *g_pVerifiedCommands = *(CInput::CVerifiedUserCmd**)( (DWORD)g_pInput + 0xC8 );
	CInput::CVerifiedUserCmd *pVerified = &g_pVerifiedCommands[ sequence_number % MULTIPLAYER_BACKUP ];
	CUserCmd *pCurCmd = g_pInput->GetUserCmd(sequence_number);
	C_BaseCombatWeapon *pWeapon = gNospread.GetBaseCombatActiveWeapon(pBaseEntity);
	int m_nTickBase = *(int*)((DWORD)pBasePlayer + 0x10D4); //109c //1080
	float curtime = m_nTickBase * g_pGlobals->interval_per_tick;
	QAngle nospreadfix;
	
	float forward	= pCurCmd->forwardmove; 
	float right		= pCurCmd->sidemove; 
	float up		= pCurCmd->upmove; 
	float ftime = g_pEngine->Time();
	Vector viewforward,viewright,viewup,aimforward,aimright,aimup,vForwardNorm,vRightNorm,vUpNorm;		
	AngleVectors(pCurCmd->viewangles, &viewforward, &viewright, &viewup);

	Vector vMove(pCurCmd->forwardmove, pCurCmd->sidemove, pCurCmd->upmove);
	float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
	QAngle qMove, qRealView(pCurCmd->viewangles);

	flYaw = DEG2RAD(pCurCmd->viewangles.y - qRealView.y + qMove.y);


	/////////////
	//SIMPLE AA//
	/////////////
	
	if( !(pCurCmd->buttons & IN_ATTACK) && !(pCurCmd->buttons & IN_ATTACK2))
	{

		//pCurCmd->viewangles.x += (float)180 + (rand() % (int)2);
	    //pCurCmd->viewangles.y -= (float)180 + (rand() % (int)2);
		if(Cvars.misc_antiaim==1)
		{
		signed int aacounter = 0;
		pCurCmd->viewangles.x = 180.0f;
		aacounter?pCurCmd->viewangles.y -= 180.0f:pCurCmd->viewangles.y -= 180.0f;
		aacounter = !aacounter;
		}else

		if(Cvars.misc_antiaim==2)
		{
		//PAA (!?)
		pCurCmd->viewangles.x = 180;
		pCurCmd->viewangles.y -= 90.0439645;
		//*bSendPackets = true;
		}

	/////////////
	//NORMALIZE//
	/////////////
	
	AngleVectors(pCurCmd->viewangles, &aimforward, &aimright, &aimup);
	gNeeded.Normalize ( viewforward, vForwardNorm ); 
	gNeeded.Normalize( viewright, vRightNorm ); 
	gNeeded.Normalize( viewup, vUpNorm ); 		
	pCurCmd->forwardmove = 	DotProduct(forward* vForwardNorm, aimforward)+DotProduct(right* vRightNorm, aimforward)+DotProduct(up* vUpNorm, aimforward); 
	pCurCmd->sidemove = DotProduct(forward* vForwardNorm, aimright)+DotProduct(right* vRightNorm, aimright)+DotProduct(up* vUpNorm, aimright); 
	pCurCmd->upmove = DotProduct(forward* vForwardNorm, aimup)+DotProduct(right* vRightNorm, aimup)+DotProduct(up* vUpNorm, aimup);
	

	}

	///////////
	//SPINBOT//
	///////////
	if( !(pCurCmd->buttons & IN_ATTACK) && !(pCurCmd->buttons & IN_ATTACK2) && Cvars.misc_spinbot==1)
	{
		pCurCmd->viewangles.y = (vec_t)(fmod(ftime / 0.1f * 360.0f,360.0f));

		/////////////
		//NORMALIZE//
		/////////////
	AngleVectors(pCurCmd->viewangles, &aimforward, &aimright, &aimup);
	gNeeded.Normalize ( viewforward, vForwardNorm ); 
	gNeeded.Normalize( viewright, vRightNorm ); 
	gNeeded.Normalize( viewup, vUpNorm ); 		
	pCurCmd->forwardmove = 	DotProduct(forward* vForwardNorm, aimforward)+DotProduct(right* vRightNorm, aimforward)+DotProduct(up* vUpNorm, aimforward); 
	pCurCmd->sidemove = DotProduct(forward* vForwardNorm, aimright)+DotProduct(right* vRightNorm, aimright)+DotProduct(up* vUpNorm, aimright); 
	pCurCmd->upmove = DotProduct(forward* vForwardNorm, aimup)+DotProduct(right* vRightNorm, aimup)+DotProduct(up* vUpNorm, aimup); 

	}
	

	///////////
	//AIMBUTZ//
	///////////
	
	if(Cvars.aim_bot==1 && Cvars.aim_key==0)
	{
	g_pAimbot.Bot(pCurCmd);
	}

	if(pCurCmd->buttons & IN_ATTACK)
	{
		//if(Cvars.aim_silent==2)
		//*bSendPackets = false; //pSilent

		//AUTOPISTOL
		if(pWeapon && Cvars.misc_autopistol==1)
		{
			float m_flNextPrimaryAttack = *(PFLOAT)((DWORD)pWeapon + 0x868); //828
			if(!(m_flNextPrimaryAttack <= curtime)) 
			pCurCmd->buttons &=~ IN_ATTACK;
		}

		
		//pNOSPREAD
		if(Cvars.rem_spread==1)
		{
		gNospread.GetSpreadFix(pCurCmd->command_number,pCurCmd->viewangles,nospreadfix);
		}

		//SHAKYNORECOIL
		if(Cvars.rem_recoil==1)
		{
		QAngle* pPunchAngle = ( QAngle* )( (DWORD)pBaseEntity + (0x0DC4 + 0x6C)); //0D7C
		pCurCmd->viewangles.x -= (	pPunchAngle->x *2.0f	);
		pCurCmd->viewangles.y -= (	pPunchAngle->y *2.0f	);
		}
		

		//pNORECOIL
		/*
		if(Cvars.rem_recoil==2)
		{
		pCurCmd->viewangles = pCurCmd->viewangles - QAngle(vCompensationRecoil.x,vCompensationRecoil.y,vCompensationRecoil.z) + nospreadfix;
		}
		*/

		if(Cvars.aim_key==1 & Cvars.aim_bot==1 & Cvars.aim_shoot == 0)
		{
		g_pAimbot.Bot(pCurCmd); //AIM KEY
		}
	}


	////////
	//BHOP//
	////////
	int iFlags = *(PINT)((DWORD)pBasePlayer + m_offsets.dw_m_fFlags());
	if( pCurCmd->buttons &IN_JUMP && !( iFlags&FL_ONGROUND ) && !( iFlags&FL_PARTIALGROUND ) & Cvars.misc_bhop==1) 
    pCurCmd->buttons &= ~IN_JUMP; 


	///////////////
	//VERIFIEDCMD//
	///////////////
	pVerified->m_cmd = *pCurCmd;
	pVerified->m_crc =  pCurCmd->GetChecksum();	
}


/////////////////
//PAINTTRAVERSE//
/////////////////

DWORD g_dwOrgPaintTraverse;
tPaintTraverse pPaintTraverse;

void __stdcall new_PaintTraverse(vgui::VPANEL vguiPanel, bool forceRepaint, bool allowForce)
{
	pPaintTraverse(vguiPanel,forceRepaint,allowForce);
	//"MatSystemTopPanel"
	int iScreenSize[2];
	g_pEngine->GetScreenSize(iScreenSize[0], iScreenSize[1]);
	int iScreenCenter[2] = { iScreenSize[0] * 0.5, iScreenSize[1] * 0.5 };
    const char* pszPanelName = g_pPanel->GetName(vguiPanel);
    if( !strcmp(pszPanelName,"MatSystemTopPanel") )
		if (g_pEngine->IsInGame())
		{
				///////////
				//DRAWESP//
				///////////
				gEsp.Esp();

				////////////
				//DRAWMENU//
				////////////
				gui.Init();
				gui.MenuDraw();
		}		
}


///////////////
//NOVISRECOIL//
///////////////
DWORD g_dwOrgFrameStageNotify;
void __stdcall new_FrameStageNotify ( ClientFrameStage_t curStage )
{
QAngle *PunchAngle, OldPunch;
C_BaseEntity *pMe;
if( curStage == FRAME_RENDER_START)
{
if(g_pEngine->IsInGame())
{
pMe = g_pEntList->GetClientEntity(g_pEngine->GetLocalPlayer())->GetBaseEntity();
PunchAngle = (QAngle*)((DWORD)pMe + (0x0DC4 + 0x6C) );
if(pMe && Cvars.rem_visrecoil==1)
{
OldPunch = *PunchAngle;
*PunchAngle = QAngle(0,0,0);
}
}
}

_asm
{
PUSH curStage
CALL g_dwOrgFrameStageNotify
}

if(curStage == FRAME_RENDER_START)
{
if(g_pEngine->IsInGame())
{
*PunchAngle = OldPunch;
}
}
}


/////////////
//SPEEDHACK//
/////////////
/*


typedef void ( __stdcall *CL_Move_t)();
static CL_Move_t CL_Move;
void __stdcall new_CL_Move()
{
	if(Cvars.misc_speedhack==1 && GetAsyncKeyState(69))
		{
		for(int i = 0; i <= 10; i++)
		CL_Move();
		}
		else
		CL_Move();
}

*/


/////////////
//HUDUPDATE//
/////////////

DWORD g_dwOrgHudUpdate;
void HookPropRecv( const CRecvProxyData *pData, void *pStruct, void *pOut )
{

static bool lookedDownLastTick[MAX_PLAYERS];

float flPitch = pData->m_Value.m_Float;

flPitch -= 0.087929;//use the compressed value that i put but it was that? LOL k sec

if ( flPitch > 180 )
{
	flPitch -= 360;
}

flPitch = clamp( flPitch, -90, 90 );

*(float*)pOut = flPitch;
}
 void __stdcall new_HudUpdate( bool bActive )
{
	_asm
	{
		PUSH bActive
		CALL g_dwOrgHudUpdate
	}

    static bool bOnce = false;
    if( !bOnce ){
		                                     
        ClientClass *pClass = g_pClient->GetAllClasses();
            while( pClass )
            {
                const char *pszName = pClass->m_pRecvTable->GetName();
               
                if( !strcmp( pszName, "DT_CSPlayer" ) )
                {

                    for( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
                    {
                        RecvProp *pProp = pClass->m_pRecvTable->GetProp( i );
                       
                        if( !strcmp( pProp->GetName(), "m_angEyeAngles[0]" ))
							pProp->SetProxyFn( HookPropRecv );       
					      

                    }
                }
                pClass = pClass->m_pNext;
}
}
}

////////
//INIT//
////////
DWORD g_dwOrgInit;
int __stdcall new_Init( CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory,CGlobalVarsBase *pGlobals )
{   
	int ret;
	int retx;
	MathLib_Init();
	ConnectTier1Libraries( &appSystemFactory, 1 );
	ConnectTier2Libraries( &appSystemFactory, 1 );
	ConnectTier3Libraries( &appSystemFactory, 1 );
DWORD* pdwClient = (DWORD*)*(DWORD*)g_pClient;
g_pGlobals = ( CGlobalVarsBase* )**( DWORD** )( pdwClient[0] + 0x3A );
pGlobals = g_pGlobals;
_asm
{
PUSH physicsFactory
PUSH appSystemFactory
PUSH pGlobals
CALL g_dwOrgInit
MOV retx, EAX
}

return ret;
}


////////
//MAIN//
////////

typedef HRESULT     (__stdcall* tEndScene)			(LPDIRECT3DDEVICE9);
typedef HRESULT     (__stdcall* tDrawPrimeIndex)	(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
typedef int			(__stdcall* tInit)				(CreateInterfaceFn,CreateInterfaceFn,CGlobalVarsBase);
typedef void(__stdcall* tCreateSecondMovement)();
static tCreateSecondMovement pSecMove;
tInit pInit;
tEndScene pEndScene;
tDrawPrimeIndex pDrawIndex;
DWORD *vTalbe = NULL;
DWORD dwEndScene = NULL;
DWORD dwDrawIndexPrimitiv = NULL;

IBaseClientDLL*	          g_pClient           = NULL;
CInput*			          g_pInput            = NULL;
IVEngineClient*	          g_pEngine           = NULL;
IClientEntityList*        g_pEntList          = NULL;
IEngineTrace*	          g_pEnginetrace      = NULL;
IVModelInfoClient*        g_pModelinfo        = NULL;
IVPhysicsDebugOverlay*    g_pDebugOverlay     = NULL;
ICvar*                    g_pCvar             = NULL;
IPanel*					  g_pPanel			  = NULL;
IMaterialSystem*		  g_pMatSystem		  = NULL;
IVRenderView*			  g_pRenderView		  = NULL;
IMatSystemSurface*        g_pMatySystemSurface = NULL;    
ISurface*				  g_pSurface		  = NULL;
IPrediction*			  g_pPrediction		  = NULL;
IVEfx*                    g_pEffects          = NULL;
IGameEventManager2*       g_pEventManager2	  = NULL;
ILocalize*			      g_pVGUILocalize	  = NULL;
CGlobalVarsBase*          g_pGlobals          = NULL;
IPhysicsSurfaceProps*     g_pPhysicAPI        = NULL;
IVModelRender*            g_pModelRender      = NULL;
HFont					  font                = NULL;
HFont					  font2               = NULL;
INetChannelInfo*		  g_pNetworkStatus	  = NULL;


DWORD WINAPI HookThread(LPVOID lpArgs)
{
	while(!GetModuleHandleA("client.dll"))
			Sleep(100);


		CreateInterfaceFn EngineFactory = Sys_GetFactory( "engine.dll" );
		CreateInterfaceFn ClientFactory = Sys_GetFactory( "client.dll" );
		CreateInterfaceFn VGUI2Factory	= Sys_GetFactory( "vgui2.dll" );
		CreateInterfaceFn MatSysFac		= Sys_GetFactory( "materialsystem.dll");
		CreateInterfaceFn VGUIFactory	= Sys_GetFactory( "vguimatsurface.dll");
		CreateInterfaceFn CvarFactory	= Sys_GetFactory( "vstdlib.dll" );
		CreateInterfaceFn PhysicFactory	= Sys_GetFactory( "vphysics.dll" );



		g_pEngine = (IVEngineClient*)EngineFactory(VENGINE_CLIENT_INTERFACE_VERSION, 0);
		DWORD* pdwEngineVMT = (DWORD*)*(DWORD*)g_pEngine;
		g_pClient = (IBaseClientDLL*)ClientFactory("VClient017", NULL);
		DWORD* pdwClientVMT = (DWORD*)*(DWORD*)g_pClient;
		gNeeded.cNetc = (CNeeded::InNet)(pdwEngineVMT[72]);
		//g_pNetworkStatus = gNeeded.cNetc();
		PDWORD Client_int  = NULL;
		memcpy(&Client_int,(void *)(g_pClient),4);
		g_pInput = (CInput*)*(PDWORD)*(PDWORD)(Client_int[21] + 0x28);

		g_pModelinfo = (IVModelInfoClient*)EngineFactory(VMODELINFO_CLIENT_INTERFACE_VERSION,0);
		g_pEnginetrace = (IEngineTrace*)EngineFactory(INTERFACEVERSION_ENGINETRACE_CLIENT,0);
		g_pEntList = (IClientEntityList*)ClientFactory(VCLIENTENTITYLIST_INTERFACE_VERSION,0);
		g_pDebugOverlay = (IVPhysicsDebugOverlay*)EngineFactory(VPHYSICS_DEBUG_OVERLAY_INTERFACE_VERSION,0); 
		g_pCvar = (ICvar*)CvarFactory(CVAR_INTERFACE_VERSION,0);
		g_pPanel = (IPanel*)VGUI2Factory(VGUI_PANEL_INTERFACE_VERSION,0);
		g_pEffects = (IVEfx*)EngineFactory(VENGINE_EFFECTS_INTERFACE_VERSION,0);
		g_pPhysicAPI    = (IPhysicsSurfaceProps*)PhysicFactory(VPHYSICS_SURFACEPROPS_INTERFACE_VERSION,0);
     	g_pMatSystem = (IMaterialSystem*)MatSysFac("VMaterialSystem080",0);
		g_pSurface			= (vgui::ISurface*)VGUIFactory(VGUI_SURFACE_INTERFACE_VERSION,0);
		g_pMatySystemSurface	= (IMatSystemSurface*)g_pSurface->QueryInterface(MAT_SYSTEM_SURFACE_INTERFACE_VERSION); 
		g_pVGUILocalize	= (ILocalize*)VGUI2Factory(VGUI_LOCALIZE_INTERFACE_VERSION,0);
		g_pEventManager2 = (IGameEventManager2*)EngineFactory(INTERFACEVERSION_GAMEEVENTSMANAGER2,0);
		g_pPrediction = (IPrediction*)ClientFactory(VCLIENT_PREDICTION_INTERFACE_VERSION,0);
		g_pModelRender = (IVModelRender*)EngineFactory(VENGINE_HUDMODEL_INTERFACE_VERSION,0);
		g_pRenderView = (IVRenderView*)EngineFactory(VENGINE_RENDERVIEW_INTERFACE_VERSION,0);
	

		DWORD* pdwPanelVMT = (PDWORD)*(PDWORD)g_pPanel;
		DWORD* pdwRenderViewVMT = (PDWORD)*(PDWORD)g_pRenderView;
		DWORD* pdwModelRender = (PDWORD)*(PDWORD)g_pModelRender;
		DWORD* pdwInput = (PDWORD)*(PDWORD)g_pInput;
		DWORD* pdwMatSysVMT = (PDWORD)*(PDWORD)g_pMatSystem;

		MEMORY_BASIC_INFORMATION Mbi;

		////////
		//INIT//
		////////
		if(VirtualQuery((LPVOID)&pdwClientVMT[1], &Mbi, sizeof(Mbi)))
		{
			VirtualProtect((LPVOID)Mbi.BaseAddress, Mbi.RegionSize, PAGE_EXECUTE_READWRITE, &Mbi.Protect);
			g_dwOrgInit = pdwClientVMT[1];
			pdwClientVMT[1] = (DWORD)&new_Init;
			VirtualProtect((LPVOID)Mbi.BaseAddress, Mbi.RegionSize, Mbi.Protect, 0);
			FlushInstructionCache(GetCurrentProcess(), (LPVOID)&pdwClientVMT[1], sizeof(DWORD));
			SecureZeroMemory(&Mbi, sizeof(Mbi));
		}

		//////////////////
		//VISUALNORECOIL//
		//////////////////
		if(VirtualQueryEx(GetCurrentProcess(),(LPVOID)&pdwClientVMT[35],&Mbi, sizeof(Mbi)))
		{
			VirtualProtectEx(GetCurrentProcess(),(LPVOID)&pdwClientVMT[35], Mbi.RegionSize, PAGE_WRITECOPY, &Mbi.Protect);
			g_dwOrgFrameStageNotify = pdwClientVMT[35];
			pdwClientVMT[35] = (DWORD)&new_FrameStageNotify;
			VirtualProtectEx(GetCurrentProcess(),(LPVOID)&pdwClientVMT[35], Mbi.RegionSize, PAGE_EXECUTE_WRITECOPY, &Mbi.Protect);
			FlushInstructionCache(GetCurrentProcess(), (LPVOID)&pdwClientVMT[35], sizeof(DWORD));
			SecureZeroMemory(&Mbi, sizeof(Mbi));
		}

		/////////////
		//CVAR INIT//
		/////////////
		
		Cvars.aim_bot = 0;
		Cvars.aim_shoot = 1;
		Cvars.aim_spot = 12;
		Cvars.esp_box = 1;
		Cvars.esp_healthbar = 1;
		Cvars.esp_visible = 1;
		Cvars.rem_visrecoil = 1;
		Cvars.rem_recoil = 0;
		Cvars.rem_spread = 1;
		Cvars.misc_speedhack = 0;
		Cvars.misc_autopistol = 0;
		Cvars.misc_bhop = 1;
		Cvars.aim_fov = 360;
		Cvars.menu_x = 150;
		Cvars.menu_y = 50;
		
		///////
		//ESP//
		///////
		gNeeded.FontInit();
		pPaintTraverse	   = (tPaintTraverse)DetourFunction((PBYTE)pdwPanelVMT[41],(PBYTE)new_PaintTraverse);


		////////
		//1337//
		////////
		if(VirtualQuery((LPVOID)&pdwInput[8], &Mbi, sizeof(Mbi)))
		{
			VirtualProtect((LPVOID)Mbi.BaseAddress, Mbi.RegionSize, PAGE_EXECUTE_READWRITE, &Mbi.Protect);
			pdwInput[8] = (DWORD)&new_GetUserCmd;
			VirtualProtect((LPVOID)Mbi.BaseAddress, Mbi.RegionSize, Mbi.Protect, 0);
			FlushInstructionCache(GetCurrentProcess(), (LPVOID)&pdwInput[8], sizeof(DWORD));
			SecureZeroMemory(&Mbi, sizeof(Mbi));
		}

		//////////////
		//CREATEMOVE//
		//////////////
		pCreateMove	   = (tCreateMove)DetourFunction((PBYTE)pdwClientVMT[21],(PBYTE)new_CreateMove);

		//////////
		//pSpeed//
		//////////
		//CL_Move = (CL_Move_t)DetourFunction((PBYTE)(PDWORD)GetModuleHandleA( "engine.dll" )+ 0x658E0,(PBYTE)new_CL_Move);

	return 0x00001337;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	DWORD tID = 0;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0,0,(LPTHREAD_START_ROUTINE)HookThread,0,0,&tID);
		return TRUE;
	}

	return FALSE;
}


#pragma optimize("", on)