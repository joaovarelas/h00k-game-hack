#pragma optimize("", off)


#include "aimbot.h"

C_GameRules * g_pGameRules;



bool SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime , const model_t *m_pModel , C_BaseEntity * pEnt )
{
	if (!m_pModel)
		return false;

	// Setup our transform.
	matrix3x4_t parentTransform;
	const QAngle &vRenderAngles = pEnt->GetRenderAngles();
	const Vector &vRenderOrigin = pEnt->GetRenderOrigin();
	AngleMatrix( vRenderAngles, parentTransform );
	parentTransform[0][3] = vRenderOrigin.x;
	parentTransform[1][3] = vRenderOrigin.y;
	parentTransform[2][3] = vRenderOrigin.z;

	// Just copy it on down baby
	studiohdr_t *pStudioHdr = g_pModelinfo->GetStudiomodel( m_pModel );
	for (int i = 0; i < pStudioHdr->numbones; i++) 
	{
		MatrixCopy( parentTransform, pBoneToWorldOut[i] );
	}

	return true;
}

void CStudioHdr::Init( const studiohdr_t *pStudioHdr, IMDLCache *mdlcache )
{
	m_pStudioHdr = pStudioHdr;

	m_pVModel = NULL;
	m_pStudioHdrCache.RemoveAll();

	if (m_pStudioHdr == NULL)
	{
		return;
	}

	if ( mdlcache )
	{
		m_pFrameUnlockCounter = mdlcache->GetFrameUnlockCounterPtr( MDLCACHE_STUDIOHDR );
		m_nFrameUnlockCounter = *m_pFrameUnlockCounter - 1;
	}

	if (m_pStudioHdr->numincludemodels == 0)
	{
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
#else
		m_ActivityToSequence.Initialize(this);
#endif
	}
	else
	{
		ResetVModel( m_pStudioHdr->GetVirtualModel() );
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
#else
		m_ActivityToSequence.Initialize(this);
#endif
	}

	m_boneFlags.EnsureCount( numbones() );
	m_boneParent.EnsureCount( numbones() );
	for (int i = 0; i < numbones(); i++)
	{
		m_boneFlags[i] = pBone( i )->flags;
		m_boneParent[i] = pBone( i )->parent;
	}
}

const virtualmodel_t * CStudioHdr::ResetVModel( const virtualmodel_t *pVModel ) const
{
	if (pVModel != NULL)
	{
		m_pVModel = (virtualmodel_t *)pVModel;
	#if defined(_WIN32) && !defined(THREAD_PROFILER)
		Assert( !pVModel->m_Lock.GetOwnerId() );
	#endif
		m_pStudioHdrCache.SetCount( m_pVModel->m_group.Count() );

		int i;
		for (i = 0; i < m_pStudioHdrCache.Count(); i++)
		{
			m_pStudioHdrCache[ i ] = NULL;
		}
		
		return const_cast<virtualmodel_t *>(pVModel);
	}
	else
	{
		m_pVModel = NULL;
		return NULL;
	}
}

CStudioHdr::CStudioHdr( void ) 
{
	// set pointer to bogus value
	m_nFrameUnlockCounter = 0;
	m_pFrameUnlockCounter = &m_nFrameUnlockCounter;
	Init( NULL );
}

void C_BaseAnimating::LockStudioHdr()
{
	AUTO_LOCK( m_StudioHdrInitLock );
	const model_t *mdl = GetModel();
	if (mdl)
	{
		m_hStudioHdr = g_pModelinfo->GetCacheHandle( mdl );
		if ( m_hStudioHdr != MDLHANDLE_INVALID )
		{
			const studiohdr_t *pStudioHdr = mdlcache->LockStudioHdr( m_hStudioHdr );
			CStudioHdr *pStudioHdrContainer = NULL;
			if ( !m_pStudioHdr )
			{
				if ( pStudioHdr )
				{
					pStudioHdrContainer = new CStudioHdr;
					pStudioHdrContainer->Init( pStudioHdr, mdlcache );
				}
				else
				{
					m_hStudioHdr = MDLHANDLE_INVALID;
				}
			}
			else
			{
				pStudioHdrContainer = m_pStudioHdr;
			}

			Assert( ( pStudioHdr == NULL && pStudioHdrContainer == NULL ) || pStudioHdrContainer->GetRenderHdr() == pStudioHdr );

			if ( pStudioHdrContainer && pStudioHdrContainer->GetVirtualModel() )
			{
				MDLHandle_t hVirtualModel = (MDLHandle_t)reinterpret_cast<intptr_t>(pStudioHdrContainer->GetRenderHdr()->virtualModel);
				mdlcache->LockStudioHdr( hVirtualModel );
			}
			m_pStudioHdr = pStudioHdrContainer; // must be last to ensure virtual model correctly set up
		}
	}
}

cAimbot g_pAimbot;

void __fastcall cAimbot::Reset()
{
	flBestDist = 99999.9f;
	nTarget = -1;
}

cAimbot::cAimbot()
{
	Reset();
}

void __fastcall cAimbot::CalculateAngle(Vector &vSource, Vector &vDestination, QAngle &qAngle)
{
	Vector vDelta(vSource - vDestination);
	float flHyp = sqrt((vDelta.x * vDelta.x) + (vDelta.y * vDelta.y));
	qAngle.x = atan(vDelta.z/flHyp) *(180.f / M_PI);
	qAngle.y = atan(vDelta.y/vDelta.x) *(180.f / M_PI);
	if(vDelta.x >= 0.f)
	qAngle.y += 180.f;
}

static CBaseEntity* GetEntityByIndex( int Index )
		{
			if( g_pEntList == NULL )
				return NULL;
	
			IClientEntity *pClient = g_pEntList->GetClientEntity( Index );
	
			return pClient->GetBaseEntity();
		}

static CBaseEntity* pLocalEntity()
		{
			if( g_pEngine == NULL )
				return NULL;
	
			return GetEntityByIndex( g_pEngine->GetLocalPlayer() );
		}

C_BaseCombatWeapon* cAimbot::GetBaseCombatActiveWeapon( C_BaseEntity* pBaseEntity )
		{
			C_BaseCombatWeapon* pActiveWeapon;

			__asm
			{
				MOV ECX,pBaseEntity
				MOV EAX,[ECX]
				CALL DWORD PTR DS:[EAX+0x360]
				MOV pActiveWeapon,EAX            
			}

			return pActiveWeapon;
		}

int cAimbot::getWeaponID(const char *weaponName){
	C_BaseCombatWeapon *m_pWeapon = GetBaseCombatActiveWeapon(pLocalEntity());
	if(!m_pWeapon)
		return WEAPON_NULL;
	if(strstr(weaponName,"_knife_t.mdl"))
		return WEAPON_KNIFE;
	if(strstr(weaponName,"_knife_ct.mdl"))
		return WEAPON_KNIFE;
	if(strstr(weaponName,"_pist_deagle.mdl"))
		return WEAPON_DEAGLE;
	if(strstr(weaponName,"_pist_elite.mdl"))
		return WEAPON_ELITES;
	if(strstr(weaponName,"_pist_fiveseven.mdl"))
		return WEAPON_FIVESEVEN;
	if(strstr(weaponName,"_pist_glock18.mdl"))
		return WEAPON_GLOCK18;
	if(strstr(weaponName,"_pist_p228.mdl"))
		return WEAPON_P228;
	if(strstr(weaponName,"_pist_usp.mdl"))
		return WEAPON_USP45;
	if(strstr(weaponName,"_rif_ak47.mdl"))
		return WEAPON_AK47;
	if(strstr(weaponName,"_rif_aug.mdl"))
		return WEAPON_AUG;
	if(strstr(weaponName,"_rif_m4a1.mdl"))
		return WEAPON_M4A1;
	if(strstr(weaponName,"_rif_famas.mdl"))
		return WEAPON_FAMAS;
	if(strstr(weaponName,"_rif_galil.mdl"))
		return WEAPON_GALIL;
	if(strstr(weaponName,"_rif_sg552.mdl"))
		return WEAPON_SG552;
	if(strstr(weaponName,"_mach_m249para.mdl"))
		return WEAPON_M249;
	if(strstr(weaponName,"_smg_tmp.mdl"))
		return WEAPON_TMP;
	if(strstr(weaponName,"_smg_mac10.mdl"))
		return WEAPON_MAC10;
	if(strstr(weaponName,"_smg_p90.mdl"))
		return WEAPON_P90;
	if(strstr(weaponName,"_smg_ump45.mdl"))
		return WEAPON_UMP45;
	if(strstr(weaponName,"_smg_mp5.mdl"))
		return WEAPON_MP5;
	if(strstr(weaponName,"_snip_g3sg1.mdl"))
		return WEAPON_G3SG1;
	if(strstr(weaponName,"_snip_scout.mdl"))
		return WEAPON_SCOUT;
	if(strstr(weaponName,"_snip_sg550.mdl"))
		return WEAPON_SG550;
	if(strstr(weaponName,"_snip_awp.mdl"))
		return WEAPON_AWP;
	if(strstr(weaponName,"_shot_xm1014.mdl"))
		return WEAPON_XM1014;
	if(strstr(weaponName,"_shot_m3super90.mdl"))
		return WEAPON_M3;
	if(strstr(weaponName,"_eq_flashbang.mdl"))
		return WEAPON_FLASHBANG;
	if(strstr(weaponName,"_eq_fraggrenade.mdl"))
		return WEAPON_HE_GRENADE;
	if(strstr(weaponName,"_eq_smokegrenade.mdl"))
		return WEAPON_SMOKE_GRENADE;
	if(strstr(weaponName,"_c4.mdl"))
		return WEAPON_C4;
	return WEAPON_NULL;
}

bool __fastcall cAimbot::HasAmmo(C_BaseEntity *pEnt)
{
	C_BaseCombatWeapon* m_pWeapon = g_pAimbot.GetBaseCombatActiveWeapon ( pEnt );
    if ( !m_pWeapon )
        return true;
	int m_iClip1 = *(int*)((DWORD)m_pWeapon + 0x8AC);//97C //86C
    if(m_iClip1 == 0)
        return false;
return true;
}

void cAimbot::MakeVector(QAngle angle, QAngle& vector){
	float pitch; 
	float yaw; 
	float tmp;           
	pitch	= (float)(angle[0] * M_PI/180); 
	yaw		= (float)(angle[1] * M_PI/180); 
	tmp		= (float) cos(pitch);
	vector[0] = (float) (-tmp * -cos(yaw)); 
	vector[1] = (float) (sin(yaw)*tmp);
	vector[2] = (float) -sin(pitch);
}

void __fastcall cAimbot::CalcAngle(Vector &src, Vector &dst, QAngle &angles){

	double delta[3] = { (src[0]-dst[0]), (src[1]-dst[1]), (src[2]-dst[2]) };
	double hyp = sqrt(delta[0]*delta[0] + delta[1]*delta[1]);
	angles[0] = (float) (atan(delta[2]/hyp) * M_RADPI);
	angles[1] = (float) (atan(delta[1]/delta[0]) * M_RADPI);
	angles[2] = 0.0f;
	if(delta[0] >= 0.0) 
	{ 
		angles[1] += 180.0f; 
	}
}

float cAimbot::GetFov( QAngle angle, Vector src, Vector dst ) 
{ 
	QAngle ang,aim; 
	float fov; 

	CalcAngle(src, dst, ang); 
	MakeVector(angle, aim); 
	MakeVector(ang, ang);      

	float mag_s = sqrt((aim[0]*aim[0]) + (aim[1]*aim[1]) + (aim[2]*aim[2])); 
	float mag_d = sqrt((aim[0]*aim[0]) + (aim[1]*aim[1]) + (aim[2]*aim[2])); 

	float u_dot_v = aim[0]*ang[0] + aim[1]*ang[1] + aim[2]*ang[2]; 

	fov = acos(u_dot_v / (mag_s*mag_d)) * (180.0 / M_PI); 

	return fov; 
}

bool __fastcall cAimbot::bHasMiscWeapon()
{
	C_BaseEntity*pLocalPlayer = (C_BaseEntity*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );
	CBaseCombatWeapon* pWeapon = g_pAimbot.GetBaseCombatActiveWeapon(pLocalPlayer);
	if(!pWeapon)
		return false;
	int iWeaponID = g_pAimbot.getWeaponID(g_pModelinfo->GetModelName(pWeapon->GetModel()));
	bool MiscWpn = ((iWeaponID == 0 || iWeaponID == 25 || iWeaponID == 26 || iWeaponID == 27 || iWeaponID == 28 || iWeaponID == 29) ? true : false);
	if(MiscWpn)
		return true;
	return false;
}

bool __fastcall cAimbot::GetVisible( Vector& vecAbsStart, Vector& vecAbsEnd, C_BaseEntity* pBaseEnt ) 
{ 
	player_info_t pinfo;
	trace_t tr;
	Ray_t ray;

	ray.Init( vecAbsStart, vecAbsEnd );
	g_pEnginetrace->TraceRay( ray, MASK_NPCWORLDSTATIC|CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX, NULL, &tr );

	if ( tr.fraction > 0.97f )
		return true;


		/*surfacedata_t *pSurfaceData = g_pPhysicAPI->GetSurfaceData( tr.surface.surfaceProps );
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
		}

		if ( tr.fraction != 1.0 )
		{
			if (  tr.allsolid /*|| tr.DidHitWorld())
				return false;
		}
		else
		{
			return true;
		}
		*/
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

bool __fastcall GetHitboxPosition ( int iHitBox, Vector &vecOrigin, QAngle &angles, int index )
{
	if( iHitBox < 0 || iHitBox >= 20 )
		return false;

	matrix3x4_t pmatrix[MAXSTUDIOBONES];

	Vector vMin, vMax;

	IClientEntity* ClientEntity = g_pEntList->GetClientEntity( index );

	if ( ClientEntity == NULL )
		return false;

	if ( ClientEntity->IsDormant() )
		return false;
	
	const model_t * model;

	model = ClientEntity->GetModel();
	if( model )
	{
		studiohdr_t *pStudioHdr = g_pModelinfo->GetStudiomodel( model);

		if ( !pStudioHdr )
			return false;

		if( ClientEntity->SetupBones( pmatrix, 128, BONE_USED_BY_HITBOX, g_pGlobals->curtime) == false )
			return false;

		mstudiohitboxset_t *set =pStudioHdr->pHitboxSet( 0 );

		if ( !set )
			return false;

		// pointer to the hitbox
		mstudiobbox_t* pbox = NULL;

		pbox = pStudioHdr->pHitbox(iHitBox, 0);
		MatrixAngles( pmatrix[ pbox->bone ], angles, vecOrigin );
		VectorTransform( pbox->bbmin, pmatrix[ pbox->bone ], vMin );
		VectorTransform( pbox->bbmax, pmatrix[ pbox->bone ], vMax );
		vecOrigin = ( vMin + vMax ) * 0.5f;
                		}
	return true;
}

void _fastcall FastVectorAngles( const Vector& forward, QAngle &angles )
{
	Assert( s_bMathlibInitialized );
	float	tmp, yaw, pitch;
	
	if (forward[1] == 0.f && forward[0] == 0.f)
	{
		yaw = 0.f;
		if (forward[2] >= 0.f)
			pitch = 270.f;
		else
			pitch = 90.f;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180.f / M_PI);
		if (yaw <= 0.f)
			yaw += 360.f;

		tmp = FastSqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180.f / M_PI);
		if (pitch <= 0.f)
			pitch += 360.f;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0.f;
}

int __fastcall GenerateRandomNumber()
{
	#define IA 16807
	#define IM 2147483647
	#define IQ 127773
	#define IR 2836
	#define NDIV (1+(IM-1)/NTAB)
	#define MAX_RANDOM_RANGE 0x7FFFFFFFUL
	#define AM (1.0/IM)
	#define EPS 1.2e-7
	#define RNMX (1.0-EPS)
    int m_idum;
    int m_iy;
    int m_iv[32];

	int j;
	int k;
	
	if (m_idum <= 0 || !m_iy)
	{
		if (-(m_idum) < 1) 
			m_idum=1;
		else 
			m_idum = -(m_idum);

		for ( j=NTAB+7; j>=0; j--)
		{
			k = (m_idum)/IQ;
			m_idum = IA*(m_idum-k*IQ)-IR*k;
			if (m_idum < 0) 
				m_idum += IM;
			if (j < NTAB)
				m_iv[j] = m_idum;
		}
		m_iy=m_iv[0];
	}
	k=(m_idum)/IQ;
	m_idum=IA*(m_idum-k*IQ)-IR*k;
	if (m_idum < 0) 
		m_idum += IM;
	j=m_iy/NDIV;
	m_iy=m_iv[j];
	m_iv[j] = m_idum;

	return m_iy;
}

int __fastcall FastRandomInt( int iLow, int iHigh )
{
	unsigned int maxAcceptable;
	unsigned int x = iHigh-iLow+1;
	unsigned int n;
	if (x <= 1 || MAX_RANDOM_RANGE < x-1)
	{
		return iLow;
	}
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE+1) % x );
	do
	{
		n = GenerateRandomNumber();
	} while (n > maxAcceptable);

	return iLow + (n % x);
}

/////////////
//PWN THEM!//
/////////////

void __fastcall cAimbot::Bot(CUserCmd* c)
{

	C_BaseEntity* pBaseEnt;
	C_BaseEntity*pLocal = (C_BaseEntity*)g_pEntList->GetClientEntity( g_pEngine->GetLocalPlayer() );

	int nHealth;
	int rand;
	Reset();
	vEyePos = gNeeded.GetEyePosition(pLocal,c); //need to update offsets?
	player_info_t pinfo;

	for(int i = 1; i <= g_pEntList->NumberOfEntities(false); i++)
	{
		pBaseEnt = gNeeded.GetEntityByIndex(i);
		if(pBaseEnt == NULL)
			continue;
		if(pBaseEnt->index == pLocal->index)
			continue;
		if(!g_pEngine->GetPlayerInfo(pBaseEnt->index,&pinfo))
			continue;

		if( stricmp( pBaseEnt->GetClientClass()->GetName(), "CCSPlayer" ) == 0 )
		{
			nHealth = *(PINT)((DWORD)pBaseEnt + 0x90);
			if(!gNeeded.IsAlive(pLocal))
				continue;
			if(!gNeeded.IsAlive(pBaseEnt))
				continue;


		    if(nHealth <= 0)
				continue;
			
			if(pBaseEnt->IsDormant())
				continue;

			if(!HasAmmo(pLocal))
				continue;

			//if(pBaseEnt->GetTeamNumber() == pLocal->GetTeamNumber()) //crash
			 //   continue;


			//alternative

			int m_TeamNum = *(int*)((DWORD)pBaseEnt + 0x98);
	     	int myTeamNum = *(int*)((DWORD)pLocal + 0x98);

			if(m_TeamNum == myTeamNum)
				continue;

			



			if(Cvars.aim_miscweap==0 && bHasMiscWeapon())
				continue;
			
			Vector myEyePos = gNeeded.GetEyePositionNormal(pLocal),eEyePos = gNeeded.GetEyePositionNormal(pBaseEnt);

			if(!gNeeded.GetVisible(myEyePos,eEyePos,pBaseEnt))
				continue;

			if(GetFov( pLocal->GetRenderAngles(), myEyePos, eEyePos ) > Cvars.aim_fov)
				continue;

			if(!GetHitboxPosition(Cvars.aim_spot, eEyePos, c->viewangles, i))
				continue; 

	
			m_vecFinal = eEyePos;
			nTarget = i;
		
	}}


	//LOL U'LL GET PWND
	if(nTarget != -1)
	{

		FastVectorAngles(m_vecFinal - vEyePos, c->viewangles);

		//////////////
		//AIM SMOOTH//
		//////////////
		if(Cvars.aim_smooth > 0)
		{
		QAngle qCurrentView;
		g_pEngine->GetViewAngles( qCurrentView );
		QAngle qDelta = c->viewangles - qCurrentView;
		if(qDelta[0]>180)  qDelta[0]-=360;
		if(qDelta[1]>180)  qDelta[1]-=360;
		if(qDelta[0]<-180) qDelta[0]+=360;
		if(qDelta[1]<-180) qDelta[1]+=360;
		c->viewangles = qCurrentView + qDelta / Cvars.aim_smooth; // MAX SMOOHT = 50
		if(c->viewangles[0] >  180) c->viewangles[0]-=360; 
		if(c->viewangles[1] >  180) c->viewangles[1]-=360; 
		if(c->viewangles[0] < -180) c->viewangles[0]+=360; 
		if(c->viewangles[1] < -180) c->viewangles[1]+=360;
		}
		


		if(Cvars.aim_silent==1)
		{
			g_pRenderView->SetMainView(m_vecFinal,c->viewangles); //SILENT SET
		}else{
			g_pEngine->SetViewAngles(c->viewangles); //NORMAL SET
		}

		if(Cvars.aim_shoot==1)
		{
			c->buttons |= IN_ATTACK; //AUTOSHOOT
		}


		Reset();
	}
}



#pragma optimize("", on)