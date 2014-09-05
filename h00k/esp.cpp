#include "esp.h"

CEsp gEsp;
Vector m_vecWorld,vScreen;

void HealthBar(int x,int y,int w,int h,DWORD dwBorderColor)
{
	gNeeded.FillRGB( x-2/1.5+1, y, 3/1.5, h-3/2+1 ,dwBorderColor);//left health bar
}

void BoundingBox(int x,int y,int w,int h,DWORD dwColor)
{
	g_pMatySystemSurface->DrawSetColor(RED(dwColor),GREEN(dwColor),BLUE(dwColor),ALPHA(dwColor));
	g_pMatySystemSurface->DrawOutlinedRect(x,y,(x + w) + 1,(y + h) + 1);
}

void DrawBoundingBox(C_BaseEntity *pEnt,DWORD colour,int index){
	Vector mon, nom;
	DWORD dwGetColor = COLORCODE(255,255,255,255);
	nom = pEnt->GetAbsOrigin();
	int flags = *(int*)((DWORD)pEnt + 0x314); //314,30
	if(flags & FL_DUCKING)
		mon = nom + Vector( 0, 0, 50.f );
	else
		mon = nom + Vector( 0, 0, 70.f );
	Vector bot, top;
	player_info_t pInfo;
	IClientEntity* pBaseEntClient;
	C_BaseEntity*pLocal = (C_BaseEntity*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );
	Vector vEyeMe = gNeeded.GetEyePositionNormal(pLocal),vEyeEnemy = gNeeded.GetEyePositionNormal(pEnt);
	pBaseEntClient = g_pEntList->GetClientEntity(index);
	g_pEngine->GetPlayerInfo(pBaseEntClient->entindex(), &pInfo);
	C_BaseEntity* pBaseEnt;
	pBaseEnt = pBaseEntClient->GetBaseEntity();
	if( gNeeded.TransformVector( nom, bot ) && gNeeded.TransformVector( mon, top ) ){
		float h = ( bot.y - top.y ) * 1/*.17375*/;
		float w = h / 5.f;
		int health = *(int*)((DWORD)pEnt + 0x90); //90
		//int armor = *(int*)((DWORD)pEnt + 0x1454 );
		float yay = 1.665;

		//////////////
		//NORMAL BOX//
		//////////////

		if(Cvars.esp_box==1)
		{
		BoundingBox(top.x - w,top.y - yay, w * 2, h, colour ); //BOX ESP
		}

		if(Cvars.esp_healthbar==1)
		{
		HealthBar(top.x - w,top.y - yay, w * 2, h + 1, COLORCODE(0,0,0,255)); //BACKGROUND BAR
		HealthBar(top.x - w,top.y - yay, w * 2, h * health / 100 + 1, colour); //HEALTH BAR
		}

		////////////
		//NAME ESP//
		////////////
		char * vIn = pInfo.name;
		wchar_t * vOut = new wchar_t[strlen(vIn)+1];
		mbstowcs_s(NULL,vOut,strlen(vIn)+1,vIn,strlen(vIn));

		//gNeeded.DrawString(top.x,top.y + h - yay, RED(colour),GREEN(colour),BLUE(colour),ALPHA(colour), true, pInfo.name );
		if(Cvars.esp_name==1)
		{
		gNeeded.DrawString2((HFont)6,bot.x - 10,bot.y, RED(colour),GREEN(colour),BLUE(colour),ALPHA(colour), vOut );
		}



		//////////////
		//WEAPON ESP//
		//////////////
		
		C_BaseCombatWeapon* m_pWeapon = gNospread.GetBaseCombatActiveWeapon( pBaseEnt );
		if ( m_pWeapon )					
		{

			const char* szWeapon = g_pModelinfo->GetModelName ( m_pWeapon->GetModel( ) );
			char TehWeapon[100];
			char m_szPrintName[100];
			strcpy( TehWeapon, szWeapon );
			if		( strstr( TehWeapon, "w_rif_ak47.mdl" ) )					{strcpy( m_szPrintName, "AK47" );}
				else if ( strstr( TehWeapon, "w_smg_tmp.mdl" ) )					{strcpy( m_szPrintName, "TMP" );}
				else if ( strstr( TehWeapon, "w_rif_aug.mdl" ) )					{strcpy( m_szPrintName, "AUG" );}
				else if ( strstr( TehWeapon, "w_snip_g3sg1.mdl" ) )					{strcpy( m_szPrintName, "G3SG1" );}
				else if ( strstr( TehWeapon, "w_shot_m3super90.mdl" ) )				{strcpy( m_szPrintName, "M3" );}
				else if ( strstr( TehWeapon, "w_smg_mac10.mdl" ) )					{strcpy( m_szPrintName, "MAC" );}
				else if ( strstr( TehWeapon, "w_smg_p90.mdl" ) )					{strcpy( m_szPrintName, "P90" );}
				else if ( strstr( TehWeapon, "w_snip_scout.mdl" ) )					{strcpy( m_szPrintName, "SCOUT" );}
				else if ( strstr( TehWeapon, "w_snip_sg550.mdl" ) )					{strcpy( m_szPrintName, "SG550" );}
				else if ( strstr( TehWeapon, "w_smg_ump45.mdl" ) )					{strcpy( m_szPrintName, "UMP45" );}
				else if ( strstr( TehWeapon, "w_snip_awp.mdl" ) )					{strcpy( m_szPrintName, "AWP" );}
				else if ( strstr( TehWeapon, "w_rif_famas.mdl" ) )					{strcpy( m_szPrintName, "FAMAS" );}
				else if ( strstr( TehWeapon, "w_rif_galil.mdl" ) )					{strcpy( m_szPrintName, "GALIL" );}
				else if ( strstr( TehWeapon, "w_rif_m4a1_silencer.mdl" ) )			{strcpy( m_szPrintName, "M4A1 Silenced" );}
				else if ( strstr( TehWeapon, "w_rif_m4a1.mdl" ) )					{strcpy( m_szPrintName, "M4A1" );}
				else if ( strstr( TehWeapon, "w_smg_mp5.mdl" ) )					{strcpy( m_szPrintName, "MP5" );}
				else if ( strstr( TehWeapon, "w_mach_m249para.mdl" ) )				{strcpy( m_szPrintName, "M249" );}
				else if ( strstr( TehWeapon, "w_rif_sg552.mdl" ) )					{strcpy( m_szPrintName, "SG552" );}
				else if ( strstr( TehWeapon, "w_shot_xm1014.mdl" ) )				{strcpy( m_szPrintName, "XM1014" );}
				else if ( strstr( TehWeapon, "w_eq_fraggrenade.mdl" ) )				{strcpy( m_szPrintName, "HE" );}
				else if ( strstr( TehWeapon, "w_eq_flashbang.mdl" ) )				{strcpy( m_szPrintName, "FLASH" );}
				else if ( strstr( TehWeapon, "w_pist_usp.mdl" ) )					{strcpy( m_szPrintName, "USP" );}
				else if ( strstr( TehWeapon, "w_pist_usp_silencer.mdl" ) )			{strcpy( m_szPrintName, "USP Silenced" );}
				else if ( strstr( TehWeapon, "w_pist_glock18.mdl" ) )				{strcpy( m_szPrintName, "GLOCK" );}
				else if ( strstr( TehWeapon, "w_pist_deagle.mdl" ) )				{strcpy( m_szPrintName, "DEAGLE" );}
				else if ( strstr( TehWeapon, "w_knife_ct.mdl" ) )					{strcpy( m_szPrintName, "KNIFE" );}
				else if ( strstr( TehWeapon, "w_knife_t.mdl" ) )					{strcpy( m_szPrintName, "KNIFE" );}
				else if ( strstr( TehWeapon, "w_pist_p228.mdl" ) )					{strcpy( m_szPrintName, "P228" );}
				else if ( strstr( TehWeapon, "w_pist_fiveseven.mdl" ) )				{strcpy( m_szPrintName, "FIVESEVEN" );}
				else if ( strstr( TehWeapon, "w_eq_smokegrenade.mdl" ) )			{strcpy( m_szPrintName, "SMOKE" );}
				else if ( strstr( TehWeapon, "w_pist_elite.mdl" ) )					{strcpy( m_szPrintName, "ELITES" );}
				else if ( strstr( TehWeapon, "w_c4.mdl" ) )							{strcpy( m_szPrintName, "***C4***" );}


				char * vIn = m_szPrintName;
				wchar_t * vOut = new wchar_t[strlen(vIn)+1];
				mbstowcs_s(NULL,vOut,strlen(vIn)+1,vIn,strlen(vIn));

				//gNeeded.DrawString(top.x,top.y + h + 11 - yay,RED(colour),GREEN(colour),BLUE(colour),ALPHA(colour),false,"%s",m_szPrintName);
				if(Cvars.esp_weapon==1)
				{
				gNeeded.DrawString2((HFont)6,bot.x - 10,bot.y + 10,RED(colour),GREEN(colour),BLUE(colour),ALPHA(colour),vOut);
				}
				
			
		}
		

	}
} 


void CEsp::Esp()
{

		C_BaseEntity*pLocal = (C_BaseEntity*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );
		DWORD colour = COLORCODE(255,26,255,255);
		for(int index = 1; index <= g_pEngine->GetMaxClients(); index++)
		{
		if (g_pEngine->IsInGame())
		{
		if( index != g_pEngine->GetLocalPlayer() )
		{

		C_BaseEntity* pBaseEnt = gNeeded.GetEntityByIndex(index);
		if(pBaseEnt == NULL)
			continue;
		if( pBaseEnt->index == pLocal->index )
			continue;

		if( stricmp( pBaseEnt->GetClientClass()->GetName(), "CCSPlayer" ) == 0 ){
		player_info_t pinfo;
		int health = *(int*)((DWORD)pBaseEnt + 0x90);
		if( health <= 0 )
			continue;
		if(!g_pEngine->GetPlayerInfo(pBaseEnt->index,&pinfo))
			continue;
		if(pBaseEnt->IsDormant())
			continue;

		//////////////
		//TEAM COLOR//
		//////////////
		
		Vector myEyePos = gNeeded.GetEyePositionNormal(pLocal),eEyePos = gNeeded.GetEyePositionNormal(pBaseEnt);
		int m_TeamNum = *(int*)((DWORD)pBaseEnt + 0x98);
		int myTeamNum = *(int*)((DWORD)pLocal + 0x98);


		if((m_TeamNum != myTeamNum) & gNeeded.GetVisible(myEyePos,eEyePos,pBaseEnt) & Cvars.esp_visible==1)
		{
			colour = COLORCODE(26,255,26,255);
		}
		else
		{
		if( m_TeamNum == 2 ) //Terrorist 
			
			{
				colour = COLORCODE(255,26,26,255);
			}

		if( m_TeamNum == 3 )// Counter-Terrorist
			
			{
				colour = COLORCODE(26,26,255,255);
			}
		}



		////////////////
		//WORLD2SCREEN//
		////////////////
		gNeeded.GetWorldSpaceCenter(pBaseEnt,m_vecWorld);
		if(gNeeded.WorldToScreen(m_vecWorld,vScreen) & gNeeded.IsAlive(pBaseEnt))
		{
		DrawBoundingBox(pBaseEnt,colour,index);
		}
		

		}}}}

		////////////
		//REMOVALS//
		////////////
		IMaterial* m_pFlashEffect = g_pMatSystem->FindMaterial( "effects/flashbang", "ClientEffect textures" );
		IMaterial* m_pFlashOverlay = g_pMatSystem->FindMaterial( "effects/flashbang_white", "ClientEffect textures" );
		IMaterial* m_pSmokeStack = g_pMatSystem->FindMaterial( "SmokeStack", "ClientEffect textures" );
		IMaterial* m_pSmokeParticle = g_pMatSystem->FindMaterial( "particle/particle_smokegrenade", "ClientEffect textures" );
		IMaterial* m_pSmokeParticle1 = g_pMatSystem->FindMaterial( "particle/particle_smokegrenade1", "ClientEffect textures" );
		IMaterial* m_pScreenSpaceFog = g_pMatSystem->FindMaterial( "particle/screenspace_fog", "ClientEffect textures" );

		if(Cvars.rem_smoke==1)
		{
		m_pSmokeStack->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		m_pSmokeParticle->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		m_pSmokeParticle1->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		m_pScreenSpaceFog->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		}

		if(Cvars.rem_flash==1)
		{
		m_pFlashEffect->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		m_pFlashOverlay->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
		}
}
