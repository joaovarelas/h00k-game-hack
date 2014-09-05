//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#ifndef CDLL_INT_H
#define CDLL_INT_H
#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "interface.h"
#include "mathlib/mathlib.h"
#include "const.h"
#include "checksum_crc.h"
#include "datamap.h"
#include "tier1/bitbuf.h"
#include "inputsystem/ButtonCode.h"

#if !defined( _X360 )
#include "xbox/xboxstubs.h"
#endif

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class ClientClass;
struct model_t;
class CSentence;
struct vrect_t;
struct cmodel_t;
class IMaterial;
class CAudioSource;
class CMeasureSection;
class SurfInfo;
class ISpatialQuery;
struct cache_user_t;
class IMaterialSystem;
class VMatrix;
struct ScreenFade_t;
struct ScreenShake_t;
class CViewSetup;
class CEngineSprite;
class CGlobalVarsBase;
class CPhysCollide;
class CSaveRestoreData;
class INetChannelInfo;
struct datamap_t;
struct typedescription_t;
class CStandardRecvProxies;
struct client_textmessage_t;
class IAchievementMgr;
class CGamestatsData;

//-----------------------------------------------------------------------------
// Purpose: This data structure is filled in by the engine when the client .dll requests information about
//  other players that the engine knows about
//-----------------------------------------------------------------------------

// Engine player info, no game related infos here
// If you change this, change the two byteswap defintions: 
// cdll_client_int.cpp and cdll_engine_int.cpp
typedef struct player_info_s
{
	DECLARE_BYTESWAP_DATADESC();
	// scoreboard information
	char			name[MAX_PLAYER_NAME_LENGTH];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[SIGNED_GUID_LEN + 1];
	// friends identification number
	uint32			friendsID;
	// friends name
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	CRC32_t			customFiles[MAX_CUSTOM_FILES];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
} player_info_t;


//-----------------------------------------------------------------------------
// Hearing info
//-----------------------------------------------------------------------------
struct AudioState_t
{
	Vector m_Origin;
	QAngle m_Angles;
	bool m_bIsUnderwater;
};


//-----------------------------------------------------------------------------
// Skybox visibility
//-----------------------------------------------------------------------------
enum SkyboxVisibility_t
{
	SKYBOX_NOT_VISIBLE = 0,
	SKYBOX_3DSKYBOX_VISIBLE,
	SKYBOX_2DSKYBOX_VISIBLE,
};


//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED=-1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,		

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

// Used by RenderView
enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED	 = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1<<0),
	RENDERVIEW_DRAWHUD		 = (1<<1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1<<2),
};

//-----------------------------------------------------------------------------
// Lightcache entry handle
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE( LightCacheHandle_t );


//-----------------------------------------------------------------------------
// Occlusion parameters
//-----------------------------------------------------------------------------
struct OcclusionParams_t
{
	float	m_flMaxOccludeeArea;
	float	m_flMinOccluderArea;
};


//-----------------------------------------------------------------------------
// Just an interface version name for the random number interface
// See vstdlib/random.h for the interface definition
// NOTE: If you change this, also change VENGINE_SERVER_RANDOM_INTERFACE_VERSION in eiface.h
//-----------------------------------------------------------------------------
#define VENGINE_CLIENT_RANDOM_INTERFACE_VERSION	"VEngineRandom001"

// change this when the new version is incompatable with the old
#define VENGINE_CLIENT_INTERFACE_VERSION		"VEngineClient013"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the engine to the client .dll
//-----------------------------------------------------------------------------
abstract_class IVEngineClient
{
public:
	virtual int				GetIntersectingSurfaces( const model_t *model, const Vector &vCenter, const float radius, const bool bOnlyVisibleSurfaces, SurfInfo *pInfos, const int nMaxInfos ) = 0;
	virtual Vector			GetLightForPoint (const Vector &pos, bool bClamp) = 0;
	virtual IMaterial			*TraceLineMaterialAndLighting( const Vector &start, const Vector &end, Vector &diffuseLightColor, Vector& baseColor ) = 0;
	virtual const char			*ParseFile( const char *data, char *token, int maxlen ) = 0;
	virtual bool				CopyFile( const char *source, const char *destination ) = 0;
	virtual void				GetScreenSize( int& width, int& height ) = 0;
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	virtual void				ClientCmd( const char *szCmdString ) = 0;
	virtual bool				GetPlayerInfo( int ent_num, player_info_t *pinfo ) = 0;
	virtual int				GetPlayerForUserID( int userID ) = 0;
	virtual client_textmessage_t *TextMessageGet( const char *pName ) = 0;
	virtual bool				Con_IsVisible( void ) = 0;
	virtual int				GetLocalPlayer( void ) = 0;
	virtual const model_t		*LoadModel( const char *pName, bool bProp = false ) = 0;
	virtual float				Time( void ) = 0; 
	virtual float				GetLastTimeStamp( void ) = 0; 
	virtual CSentence			*GetSentence( CAudioSource *pAudioSource ) = 0;
	virtual float				GetSentenceLength( CAudioSource *pAudioSource ) = 0;
	virtual bool				IsStreaming( CAudioSource *pAudioSource ) const = 0;
	virtual void				GetViewAngles( QAngle& va ) = 0;
	virtual void				SetViewAngles( QAngle& va ) = 0;
	virtual int				GetMaxClients( void ) = 0;
	virtual	const char			*Key_LookupBinding( const char *pBinding ) = 0;
	virtual const char			*Key_BindingForKey( ButtonCode_t &code ) = 0;
	virtual void				StartKeyTrapMode( void ) = 0;
	virtual bool				CheckDoneKeyTrapping( ButtonCode_t &code ) = 0;
	virtual bool				IsInGame( void ) = 0;
	virtual bool				IsConnected( void ) = 0;
	virtual bool				IsDrawingLoadingImage( void ) = 0;
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	virtual void				Con_NXPrintf( const struct con_nprint_s *info, const char *fmt, ... ) = 0;
	virtual int				IsBoxVisible( const Vector& mins, const Vector& maxs ) = 0;
	virtual int				IsBoxInViewCluster( const Vector& mins, const Vector& maxs ) = 0;
	virtual bool				CullBox( const Vector& mins, const Vector& maxs ) = 0;
	virtual void				Sound_ExtraUpdate( void ) = 0;
	virtual const char			*GetGameDirectory( void ) = 0;
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;
	virtual const VMatrix& 		WorldToViewMatrix() = 0;
	virtual int					GameLumpVersion( int lumpId ) const = 0;
	virtual int					GameLumpSize( int lumpId ) const = 0;
	virtual bool				LoadGameLump( int lumpId, void* pBuffer, int size ) = 0;
	virtual int					LevelLeafCount() const = 0;
	virtual ISpatialQuery*		GetBSPTreeQuery() = 0;

	virtual void		LinearToGamma( float* linear, float* gamma ) = 0;
	virtual float		LightStyleValue( int style ) = 0;
	virtual void		ComputeDynamicLighting( const Vector& pt, const Vector* pNormal, Vector& color ) = 0;
	virtual void		GetAmbientLightColor( Vector& color ) = 0;
	virtual int			GetDXSupportLevel() = 0;
	virtual bool        SupportsHDR() = 0;
	virtual void		Mat_Stub( IMaterialSystem *pMatSys ) = 0;
	virtual void		GetChapterName( char *pchBuff, int iMaxLength ) = 0;
	virtual char const	*GetLevelName( void ) = 0;
	virtual struct IVoiceTweak_s *GetVoiceTweakAPI( void ) = 0;
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	virtual void		FireEvents() = 0;
	virtual int			GetLeavesArea( int *pLeaves, int nLeaves ) = 0;
	virtual bool		DoesBoxTouchAreaFrustum( const Vector &mins, const Vector &maxs, int iArea ) = 0;
	virtual void		SetAudioState( const AudioState_t& state ) = 0;
	virtual int			SentenceGroupPick( int groupIndex, char *name, int nameBufLen ) = 0;
	virtual int			SentenceGroupPickSequential( int groupIndex, char *name, int nameBufLen, int sentenceIndex, int reset ) = 0;
	virtual int			SentenceIndexFromName( const char *pSentenceName ) = 0;
	virtual const char *SentenceNameFromIndex( int sentenceIndex ) = 0;
	virtual int			SentenceGroupIndexFromName( const char *pGroupName ) = 0;
	virtual const char *SentenceGroupNameFromIndex( int groupIndex ) = 0;
	virtual float		SentenceLength( int sentenceIndex ) = 0;
	virtual void		ComputeLighting( const Vector& pt, const Vector* pNormal, bool bClamp, Vector& color, Vector *pBoxColors=NULL ) = 0;
	virtual void		ActivateOccluder( int nOccluderIndex, bool bActive ) = 0;
	virtual bool		IsOccluded( const Vector &vecAbsMins, const Vector &vecAbsMaxs ) = 0;
	virtual void		*SaveAllocMemory( size_t num, size_t size ) = 0;
	virtual void		SaveFreeMemory( void *pSaveMem ) = 0;
	virtual INetChannelInfo	*GetNetChannelInfo( void ) = 0;
	virtual void		DebugDrawPhysCollide( const CPhysCollide *pCollide, IMaterial *pMaterial, matrix3x4_t& transform, const color32 &color ) = 0;
	virtual void		CheckPoint( const char *pName ) = 0;
	virtual void		DrawPortals() = 0;
	virtual bool		IsPlayingDemo( void ) = 0;
	virtual bool		IsRecordingDemo( void ) = 0;
	virtual bool		IsPlayingTimeDemo( void ) = 0;

	// UNKNOWN RETURN VALUES - NEW FUNCTIONS
	virtual void		*GetDemoRecordingTick ( void ) = 0;
	virtual void		*GetDemoPlaybackTick ( void ) = 0;
	virtual void		*GetDemoPlaybackStartTick ( void ) = 0;
	virtual void		*GetDemoPlaybackTimeScale ( void ) = 0;
	virtual void		*GetDemoPlaybackTotalTicks ( void ) = 0;
	// END

	virtual bool		IsPaused( void ) = 0;
	virtual bool		IsTakingScreenshot( void ) = 0;
	virtual bool		IsHLTV( void ) = 0;

	// UNKNOWN RETURN VALUES - NEW FUNCTIONS
	virtual bool		IsReplay( void ) = 0;
	virtual void		*DownloadReplay ( int unknown1 ) = 0;
	virtual bool		IsReplayEnabled ( void ) = 0;
	virtual bool		IsReplayRecording ( void ) = 0;
	virtual void		*GetReplayRecordingTick ( void ) = 0;
	// END

	virtual bool		IsLevelMainMenuBackground( void ) = 0;
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0;
	virtual void		*GetVideoModes ( int& iUnknown, void *vmode_s ) = 0;
	virtual void		SetOcclusionParameters( const OcclusionParams_t &params ) = 0;
	virtual void		GetUILanguage( char *dest, int destlen ) = 0;
	virtual SkyboxVisibility_t IsSkyboxVisibleFromPoint( const Vector &vecPoint ) = 0;
	virtual const char*	GetMapEntitiesString() = 0;
	virtual bool		IsInEditMode( void ) = 0;
	virtual float		GetScreenAspectRatio() = 0;
	virtual bool		REMOVED_SteamRefreshLogin( const char *password, bool isSecure ) = 0;
	virtual bool		REMOVED_SteamProcessCall( bool & finished ) = 0;

	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)
	virtual void			GrabPreColorCorrectedFrame( int x, int y, int width, int height ) = 0;
	virtual bool			IsHammerRunning( ) const = 0;
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;
	virtual bool			MapHasHDRLighting(void) = 0;
	virtual int			GetAppID() = 0;
	virtual Vector		GetLightForPointFast(const Vector &pos, bool bClamp) = 0;
	virtual void			ClientCmd_Unrestricted( const char *szCmdString ) = 0;
	virtual void			SetRestrictServerCommands( bool bRestrict ) = 0;
	virtual void			SetRestrictClientCommands( bool bRestrict ) = 0;
	virtual void			SetOverlayBindProxy( int iOverlayID, void *pBindProxy ) = 0;
	virtual bool			CopyFrameBufferToMaterial( const char *pMaterialName ) = 0;
	virtual void			ChangeTeam( const char *pTeamName ) = 0;
	virtual void			ReadConfiguration( const bool readDefault = false ) = 0; 
	virtual void			SetAchievementMgr( IAchievementMgr *pAchievementMgr ) = 0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;
	virtual bool			MapLoadFailed( void ) = 0;
	virtual void			SetMapLoadFailed( bool bState ) = 0;
	virtual bool			IsLowViolence() = 0;
	virtual const char		*GetMostRecentSaveGame( void ) = 0;
	virtual void			SetMostRecentSaveGame( const char *lpszFilename ) = 0;
	virtual void			StartXboxExitingProcess() = 0;
	virtual bool			IsSaveInProgress() = 0;
	virtual uint			OnStorageDeviceAttached( void ) = 0;
	virtual void			OnStorageDeviceDetached( void ) = 0;
	virtual void			ResetDemoInterpolation( void ) = 0;
	virtual void SetGamestatsData( CGamestatsData *pGamestatsData ) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;

	// New functions
	virtual void			LoadFilmmaker ( void ) = 0;
	virtual void			UnloadFilmmaker ( void ) = 0;
	virtual void			*GetMouseDelta ( int&, int&, bool ) = 0;
	virtual void			ServerCmdKeyValues ( void* ) = 0;
	virtual bool			IsInCommentaryMode ( void ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the client .dll back to the engine
//-----------------------------------------------------------------------------
abstract_class IBaseClientDLL
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init( CreateInterfaceFn appSystemFactory, 
									CreateInterfaceFn physicsFactory,
									CGlobalVarsBase *pGlobals ) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
		// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0;
	
	virtual void			something1( void ) = 0;
	virtual void			something2( void ) = 0;

	// Called at the start of each level change
	virtual void			LevelInitPreEntity( char const* pMapName ) = 0;

	virtual void			LevelInitPostEntity( ) = 0;
	// Called at the end of a level
	virtual void			LevelShutdown( void ) = 0;

	// Request a pointer to the list of client datatable classes
	virtual ClientClass		*GetAllClasses( void ) = 0;

	// Called once per level to re-initialize any hud element drawing stuff
	virtual int				HudVidInit( void ) = 0;
	// Called by the engine when gathering user input
	virtual void			HudProcessInput( bool bActive ) = 0;
	// Called oncer per frame to allow the hud elements to think
	virtual void			HudUpdate( bool bActive ) = 0;
	// Reset the hud elements to their initial states
	virtual void			HudReset( void ) = 0;
	// Display a hud text message
	virtual void			HudText( const char * message ) = 0;

	// Mouse Input Interfaces
	// Activate the mouse (hides the cursor and locks it to the center of the screen)
	virtual void			IN_ActivateMouse( void ) = 0;
	// Deactivates the mouse (shows the cursor and unlocks it)
	virtual void			IN_DeactivateMouse( void ) = 0;
	// This is only called during extra sound updates and just accumulates mouse x, y offets and recenters the mouse.
	//  This call is used to try to prevent the mouse from appearing out of the side of a windowed version of the engine if 
	//  rendering or other processing is taking too long
	virtual void			IN_Accumulate (void) = 0;
	// Reset all key and mouse states to their initial, unpressed state
	virtual void			IN_ClearStates (void) = 0;
	// If key is found by name, returns whether it's being held down in isdown, otherwise function returns false
	virtual bool			IN_IsKeyDown( const char *name, bool& isdown ) = 0;
	// Raw keyboard signal, if the client .dll returns 1, the engine processes the key as usual, otherwise,
	//  if the client .dll returns 0, the key is swallowed.
	//virtual int				IN_KeyEvent( int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding ) = 0;
	int	 (__stdcall *IN_KeyEvent)( int eventcode, int keynum, const char *pszCurrentBinding );

	// This function is called once per tick to create the player CUserCmd (used for prediction/physics simulation of the player)
	// Because the mouse can be sampled at greater than the tick interval, there is a separate input_sample_frametime, which
	//  specifies how much additional mouse / keyboard simulation to perform.
	virtual void			CreateMove ( 
								int sequence_number,			// sequence_number of this cmd
								float input_sample_frametime,	// Frametime for mouse input sampling
								bool active ) = 0;				// True if the player is active (not paused)
								 		
	// If the game is running faster than the tick_interval framerate, then we do extra mouse sampling to avoid jittery input
	//  This code path is much like the normal move creation code, except no move is created
	virtual void			ExtraMouseSample( float frametime, bool active ) = 0;

	// Encode the delta (changes) between the CUserCmd in slot from vs the one in slot to.  The game code will have
	//  matching logic to read the delta.
	virtual bool			WriteUsercmdDeltaToBuffer( bf_write *buf, int from, int to, bool isnewcommand ) = 0;
	// Demos need to be able to encode/decode CUserCmds to memory buffers, so these functions wrap that
	virtual void			EncodeUserCmdToBuffer( bf_write& buf, int slot ) = 0;
	virtual void			DecodeUserCmdFromBuffer( bf_read& buf, int slot ) = 0;

	// Set up and render one or more views (e.g., rear view window, etc.).  This called into RenderView below
	virtual void			View_Render( vrect_t *rect ) = 0;

	// Allow engine to expressly render a view (e.g., during timerefresh)
	// See IVRenderView.h, PushViewFlags_t for nFlags values
	virtual void			RenderView( const CViewSetup &view, int nClearFlags, int whatToDraw ) = 0;

	// Apply screen fade directly from engine
	virtual void			View_Fade( ScreenFade_t *pSF ) = 0;

	// The engine has parsed a crosshair angle message, this function is called to dispatch the new crosshair angle
	virtual void			SetCrosshairAngle( const QAngle& angle ) = 0;

	// Sprite (.spr) model handling code
	// Load a .spr file by name
	virtual void			InitSprite( CEngineSprite *pSprite, const char *loadname ) = 0;
	// Shutdown a .spr file
	virtual void			ShutdownSprite( CEngineSprite *pSprite ) = 0;
	// Returns sizeof( CEngineSprite ) so the engine can allocate appropriate memory
	virtual int				GetSpriteSize( void ) const = 0;

	// Called when a player starts or stops talking.
	// entindex is -1 to represent the local client talking (before the data comes back from the server). 
	// entindex is -2 to represent the local client's voice being acked by the server.
	// entindex is GetPlayer() when the server acknowledges that the local client is talking.
	virtual void			VoiceStatus( int entindex, qboolean bTalking ) = 0;

	// Networked string table definitions have arrived, allow client .dll to 
	//  hook string changes with a callback function ( see INetworkStringTableClient.h )
	virtual void			InstallStringTableCallback( char const *tableName ) = 0;

	// Notification that we're moving into another stage during the frame.
	virtual void			FrameStageNotify( ClientFrameStage_t curStage ) = 0;

	// The engine has received the specified user message, this code is used to dispatch the message handler
	virtual bool			DispatchUserMessage( int msg_type, bf_read &msg_data ) = 0;

	// Save/restore system hooks
	virtual CSaveRestoreData  *SaveInit( int size ) = 0;
	virtual void			SaveWriteFields( CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int ) = 0;
	virtual void			SaveReadFields( CSaveRestoreData *, const char *, void *, datamap_t *, typedescription_t *, int ) = 0;
	virtual void			PreSave( CSaveRestoreData * ) = 0;
	virtual void			Save( CSaveRestoreData * ) = 0;
	virtual void			WriteSaveHeaders( CSaveRestoreData * ) = 0;
	virtual void			ReadRestoreHeaders( CSaveRestoreData * ) = 0;
	virtual void			Restore( CSaveRestoreData *, bool ) = 0;
	virtual void			DispatchOnRestore() = 0;

	// Hand over the StandardRecvProxies in the client DLL's module.
	virtual CStandardRecvProxies* GetStandardRecvProxies() = 0;

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshot( const char *pFilename ) = 0;

	// Given a list of "S(wavname) S(wavname2)" tokens, look up the localized text and emit
	//  the appropriate close caption if running with closecaption = 1
	virtual void			EmitSentenceCloseCaption( char const *tokenstream ) = 0;
	// Emits a regular close caption by token name
	virtual void			EmitCloseCaption( char const *captionname, float duration ) = 0;

	// Returns true if the client can start recording a demo now.  If the client returns false,
	// an error message of up to length bytes should be returned in errorMsg.
	virtual bool			CanRecordDemo( char *errorMsg, int length ) const = 0;

	// Added interface

	// save game screenshot writing
	virtual void			WriteSaveGameScreenshotOfSize( const char *pFilename, int width, int height ) = 0;

	// Gets the current view
	virtual bool			GetPlayerView( CViewSetup &playerView ) = 0;

	// Matchmaking
	virtual void			SetupGameProperties( CUtlVector< XUSER_CONTEXT > &contexts, CUtlVector< XUSER_PROPERTY > &properties ) = 0;
	virtual uint			GetPresenceID( const char *pIDName ) = 0;
	virtual const char		*GetPropertyIdString( const uint id ) = 0;
	virtual void			GetPropertyDisplayString( uint id, uint value, char *pOutput, int nBytes ) = 0;
#ifdef _WIN32
	virtual void			StartStatsReporting( HANDLE handle, bool bArbitrated ) = 0;
#endif

	virtual void			InvalidateMdlCache() = 0;

	virtual void			IN_SetSampleTime( float frametime ) = 0;

};

#define CLIENT_DLL_INTERFACE_VERSION		"VClient016"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the client .dll back to the engine for specifying shared .dll IAppSystems (e.g., ISoundEmitterSystem)
//-----------------------------------------------------------------------------
abstract_class IClientDLLSharedAppSystems
{
public:
	virtual int	Count() = 0;
	virtual char const *GetDllName( int idx ) = 0;
	virtual char const *GetInterfaceName( int idx ) = 0;
};

#define CLIENT_DLL_SHARED_APPSYSTEMS		"VClientDllSharedAppSystems001"

#endif // CDLL_INT_H
