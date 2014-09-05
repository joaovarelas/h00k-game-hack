#include "Nospread.h"
cNospread gNospread;


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

C_BaseCombatWeapon* cNospread::GetBaseCombatActiveWeapon( C_BaseEntity* pBaseEntity )
		{
			C_BaseCombatWeapon* pActiveWeapon;

			__asm
			{
				MOV ECX,pBaseEntity
				MOV EAX,[ECX]
				CALL DWORD PTR DS:[EAX+0x360]
				MOV pActiveWeapon,EAX    //offset name!?        
			}

			return pActiveWeapon;
		}

static float GetWeaponSpread( C_BaseCombatWeapon* pWeapon )
		{
			float flResult = 0.0f;

			__asm
			{
				MOV EAX, 0x5B8;
				MOV ESI, pWeapon;
				MOV EDX, DWORD PTR DS:[ESI];
				MOV EAX, DWORD PTR DS:[EDX + EAX];
				MOV ECX, ESI;
				CALL EAX;
				FSTP flResult;
			}

			return flResult;
		}


static float GetWeaponCone( C_BaseCombatWeapon* pWeapon )
		{
			float flResult = 0.0f;

			__asm
			{
				MOV EAX, 0x5BC;
				MOV ESI, pWeapon;
				MOV EDX, DWORD PTR DS:[ESI];
				MOV EAX, DWORD PTR DS:[EDX + EAX];
				MOV ECX, ESI;
				CALL EAX;
				FSTP flResult;
			}

			return flResult;
		}

static VOID UpdateAccuracyPenalty( C_BaseCombatWeapon* pWeapon )
		{
			__asm
			{
				MOV EAX, 0x5C0
				MOV ESI, pWeapon
				MOV EDX, DWORD PTR DS:[ ESI ]
				MOV EAX, DWORD PTR DS:[ EDX + EAX ]
				MOV ECX, ESI
				CALL EAX
			}
		}

static VOID GetSpreadXY( UINT seed, Vector& vec )
		{
			int iSeed = seed & 255;
			iSeed++;
			RandomSeed(iSeed + 1);

			Vector vSpread;

			C_BaseCombatWeapon* m_pWeapon = gNospread.GetBaseCombatActiveWeapon( pLocalEntity() );

			if ( !m_pWeapon )
				return;

			//UpdateAccuracyPenalty( m_pWeapon );

			vSpread.x = GetWeaponSpread(m_pWeapon);
			vSpread.y = GetWeaponCone(m_pWeapon);

			//UpdateAccuracyPenalty( m_pWeapon );

			float flA = RandomFloat( 0.0f, 2.0f * M_PI );
			float flB = RandomFloat( 0.0f, vSpread.x );
			float flC = RandomFloat( 0.0f, 2.0f * M_PI );
			float flD = RandomFloat( 0.0f, vSpread.y );

			//UpdateAccuracyPenalty( m_pWeapon );

			vec.x = ( cos ( flA ) * flB ) + ( cos ( flC ) * flD );
			vec.y = ( sin ( flA ) * flB ) + ( sin ( flC) * flD );
	
			return;
		}

VOID cNospread::GetSpreadFix( UINT seed, QAngle& vIn, QAngle& vOut )
		{
			Vector forward, right, up, vecDir;
			Vector view, spread;
			QAngle dest;
			AngleVectors( vIn, &forward, &right, &up );
			GetSpreadXY( seed, spread );

			view.x = forward.x + spread.x * right.x + spread.y * up.x;
			//view.x = 8192.0f * vecDir.x;

			view.y = forward.y + spread.x * right.y + spread.y * up.y;
			//view.y = 8192.0f * vecDir.y;

			view.z = forward.z + spread.x * right.z + spread.y * up.z;
			//view.z = 8192.0f * vecDir.z;

			VectorAngles( view, dest );
			vOut.x = vIn.x - dest.x;
			vOut.y = vIn.y - dest.y;
	
			return;
		}
