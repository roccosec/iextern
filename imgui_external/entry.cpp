
#include <windows.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include "Memory.h"
#include "vector.h"
#include "conio.h"
#include "string"
#include "wndproc.hpp"
#include "skins.hpp"
#include "nbqmemory.hpp"

#pragma comment(lib, "d3d9.lib")

NBQMemory nbqmem; // for skinchanger

typedef unsigned char uint8_t;
bool scale_form = false;
bool ssd = false;
template <typename T, size_t N>

size_t countof(T(&array)[N])
{
	return N;
}

DWORD dwLocalPlayer;
DWORD dwEntityList;
DWORD dwGlow;
DWORD dwJump; // these offsets are auto-updating
DWORD dwForceAttack;
DWORD dwClientState_ViewAngles;
DWORD dwClientState;
DWORD dwClientState_state;

DWORD dwTeam = 0xF0;
DWORD dwDormant = 0xE9;
DWORD dwFlags = 0x100;
DWORD m_iShotsFired = 0xA2E0; // these offsets we need update manually
DWORD m_aimPunchAngle = 0x301C;
DWORD m_iCrosshairId = 0xB2DC;
DWORD m_lifeState = 0x25B;
DWORD DwEntitySize = 0x10;
DWORD m_bSpotted = 0x939;
 
//for functions
bool glow = false;
bool bhop = false;
bool norecoil = false;
bool trigger = false;
bool radar = false; 
bool fake = true; // <-- not a function :D
//bool chams = false;

//for knife changer
int kmodelid = 520;
int kskinid = 1;

//for skin changer
int deagle_skinid;
int glock_skinid;
int ak47_skinid;
int awp_skinid;
int m4a1s_skinid;
int usp_skinid;
int m4a4_skinid;
int scar20_skinid;
int scout_skinid;
int aug_skinid;
int revolver_skinid;
int cz75_skinid;
int p250_skinid;
int sg553_skinid;
int nova_skinid;
int mp9_skinid;
int mp7_skinid;
int p2000_skinid;
int tec9_skinid;
int sawedoff_skinid;
int negev_skinid;
int mag7_skinid;
int bizon_skinid;
int xm1014_skinid;
int ump45_skinid;
int mp5sd_skinid;
int p90_skinid;
int mac10_skinid;
int m249_skinid;
int galil_skinid;
int g3sg1_skinid;
int famas_skinid;
int fiveseven_skinid;
int dualberettas_skinid;

//for key-functions
int bhopkey = VK_SPACE;
int triggerkey = VK_LMENU;
int disablechangerkey = VK_F9;
int panickey = VK_DELETE;

//for other functions to work
float red;
float green;
float blue;
float reds;
float greens;
float blues;
float skalercs;
float tbdelay = 25;
float tbspraydelay = 1;
float allskinswear = 0.0001f;

//for FOV function
int vmfov;

int GetKnifeID( const short itemDef )
{
	switch ( itemDef )
	{
	default:
	case WEAPON_KNIFE_BAYONET:
		return 0;
	case WEAPON_KNIFE_FLIP:
		return 1;
	case WEAPON_KNIFE_GUT:
		return 2;
	case WEAPON_KNIFE_KARAMBIT:
		return 3;
	case WEAPON_KNIFE_M9_BAYONET:
		return 4;
	case WEAPON_KNIFE_TACTICAL:
		return 5;
	case WEAPON_KNIFE_FALCHION:
		return 6;
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return 7;
	case WEAPON_KNIFE_BUTTERFLY:
		return 8;
	case WEAPON_KNIFE_PUSH:
		return 9;
	case WEAPON_KNIFE_URSUS:
		return 10;
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return 11;
	case WEAPON_KNIFE_STILETTO:
		return 12;
	case WEAPON_KNIFE_WIDOWMAKER:
		return 13;
	}
}

void skinsX( HANDLE csgo, DWORD client, short itemDef, DWORD paintKit )
{
	const int itemIDHigh = -1;
	const int entityQuality = 3;
	const float fallbackWear = allskinswear; //const float fallbackWear = 0.0001f;

	int knifeID = GetKnifeID( itemDef );
	int knifeIDOffset = knifeID < 10 ? 0 : 1; /* precache offset id by 1 for new knives */

	DWORD cachedPlayer = 0;
	DWORD modelIndex = 0;

	while ( !GetAsyncKeyState( disablechangerkey ) ) 
	{
		DWORD localPlayer = nbqmem.ReadMemory<DWORD>( csgo, client + dwLocalPlayer ); // ( csgo, client + m_dwLocalPlayer );
		if ( localPlayer == 0 ) /* localplayer is not connected to any server (unreliable method) */
		{
			modelIndex = 0;
			continue;
		}
		else if ( localPlayer != cachedPlayer ) /* localplayer changed by server switch / demo record */
		{
			modelIndex = 0;
			cachedPlayer = localPlayer;
		}

		if ( paintKit > 0 && modelIndex > 0 )
		{
			for ( int i = 0; i < 8; i++ )
			{
				/* handle to weapon entity in the current slot */
				DWORD myWeapons = nbqmem.ReadMemory<DWORD>( csgo, localPlayer + m_hMyWeapons + i * 0x4 ) & 0xfff;
				DWORD weaponEntity = nbqmem.ReadMemory<DWORD>( csgo, client + dwEntityList + ( myWeapons - 1 ) * 0x10 ); // ( csgo, client + m_dwEntityList + ( myWeapons - 1 ) * 0x10 )
				if ( weaponEntity == 0 ) { continue; }

				/* id of the weapon in the current slot */
				short weaponID = nbqmem.ReadMemory<short>( csgo, weaponEntity + m_iItemDefinitionIndex );

				DWORD fallbackPaint = paintKit;
				if ( weaponID == 1 ) { fallbackPaint = deagle_skinid; } //deagle_skinid
				else if ( weaponID == 4 ) { fallbackPaint = glock_skinid; } //glock_skinid
				else if ( weaponID == 7 ) { fallbackPaint = ak47_skinid; } //ak47_skinid
				else if ( weaponID == 9 ) { fallbackPaint = awp_skinid; } //awp_skinid
				else if ( weaponID == 60 ) { fallbackPaint = m4a1s_skinid; } //m4a1s_skinid
				else if ( weaponID == 61 ) { fallbackPaint = usp_skinid; } //usp_skinid
				else if ( weaponID == 16 ) { fallbackPaint = m4a4_skinid; } //m4a4_skinid
				else if ( weaponID == 38 ) { fallbackPaint = scar20_skinid; } //scar20_skinid
				else if ( weaponID == 40 ) { fallbackPaint = scout_skinid; } //scout_skinid
				else if ( weaponID == 8 ) { fallbackPaint = aug_skinid; } //aug_skinid
				else if ( weaponID == 64 ) { fallbackPaint = revolver_skinid; } //revolver_skinid
				else if ( weaponID == 63 ) { fallbackPaint = cz75_skinid; } //cz75_skinid
				else if ( weaponID == 36 ) { fallbackPaint = p250_skinid; } //p250_skinid
				else if ( weaponID == 39 ) { fallbackPaint = sg553_skinid; } //sg553_skinid
				else if ( weaponID == 35 ) { fallbackPaint = nova_skinid; } //nova_skinid
				else if ( weaponID == 34 ) { fallbackPaint = mp9_skinid; } //mp9_skinid
				else if ( weaponID == 33 ) { fallbackPaint = mp7_skinid; } //mp7_skinid
				else if ( weaponID == 32 ) { fallbackPaint = p2000_skinid; } //p2000_skinid
				else if ( weaponID == 30 ) { fallbackPaint = tec9_skinid; } //tec9_skinid
				else if ( weaponID == 29 ) { fallbackPaint = sawedoff_skinid; } //sawedoff_skinid
				else if ( weaponID == 28 ) { fallbackPaint = negev_skinid; } //negev_skinid
				else if ( weaponID == 27 ) { fallbackPaint = mag7_skinid; } //mag7_skinid
				else if ( weaponID == 26 ) { fallbackPaint = bizon_skinid; } //bizon_skinid
				else if ( weaponID == 25 ) { fallbackPaint = xm1014_skinid; } //xm1014_skinid
				else if ( weaponID == 24 ) { fallbackPaint = ump45_skinid; } //ump45_skinid
				else if ( weaponID == 23 ) { fallbackPaint = mp5sd_skinid; } //mp5sd_skinid
				else if ( weaponID == 19 ) { fallbackPaint = p90_skinid; } //p90_skinid
				else if ( weaponID == 17 ) { fallbackPaint = mac10_skinid; } //mac10_skinid
				else if ( weaponID == 14 ) { fallbackPaint = m249_skinid; } //m249_skinid
				else if ( weaponID == 13 ) { fallbackPaint = galil_skinid; } //galil_skinid
				else if ( weaponID == 11 ) { fallbackPaint = g3sg1_skinid; } //g3sg1_skinid
				else if ( weaponID == 10 ) { fallbackPaint = famas_skinid; } //famas_skinid
				else if ( weaponID == 3 ) { fallbackPaint = fiveseven_skinid; } //fiveseven_skinid
				else if ( weaponID == 2 ) { fallbackPaint = dualberettas_skinid; } //dualberettas_skinid
				else if ( weaponID != WEAPON_KNIFE && weaponID != WEAPON_KNIFE_T && weaponID != itemDef ) { continue; }
				else
				{
					/* knife-specific memory writes */
					nbqmem.WriteMemory<short>( csgo, weaponEntity + m_iItemDefinitionIndex, itemDef );
					nbqmem.WriteMemory<DWORD>( csgo, weaponEntity + m_nModelIndex, modelIndex );
					nbqmem.WriteMemory<DWORD>( csgo, weaponEntity + m_iViewModelIndex, modelIndex );
					nbqmem.WriteMemory<int>( csgo, weaponEntity + m_iEntityQuality, entityQuality );
				}

				/* memory writes necessary for both knives and other weapons in slots */
				nbqmem.WriteMemory<int>( csgo, weaponEntity + m_iItemIDHigh, itemIDHigh );
				nbqmem.WriteMemory<DWORD>( csgo, weaponEntity + m_nFallbackPaintKit, fallbackPaint );
				nbqmem.WriteMemory<float>( csgo, weaponEntity + m_flFallbackWear, fallbackWear );
			}
		}

		/* handle to weapon entity we are currently holding in hands */
		DWORD activeWeapon = nbqmem.ReadMemory<DWORD>( csgo, localPlayer + m_hActiveWeapon ) & 0xfff;
		DWORD weaponEntity = nbqmem.ReadMemory<DWORD>( csgo, client + dwEntityList + ( activeWeapon - 1 ) * 0x10 ); // ( csgo, client + m_dwEntityList + ( activeWeapon - 1 ) * 0x10 )
		if ( weaponEntity == 0 ) { continue; }

		/* id of weapon we are currently holding in hands */
		short weaponID = nbqmem.ReadMemory<short>( csgo, weaponEntity + m_iItemDefinitionIndex );

		/* viewmodel id of the weapon in our hands (default ct knife should usually be around 300) */
		int weaponViewModelID = nbqmem.ReadMemory<int>( csgo, weaponEntity + m_iViewModelIndex );

		/* a way to calculate the correct modelindex for different servers / maps / teams */
		if ( weaponID == WEAPON_KNIFE && weaponViewModelID > 0 )
		{
			modelIndex = weaponViewModelID + precache_bayonet_ct + 3 * knifeID + knifeIDOffset; //modelIndex = weaponViewModelID + precache_bayonet_ct + 3 * knifeID + knifeIDOffset;
		}
		else if ( weaponID == WEAPON_KNIFE_T && weaponViewModelID > 0 )
		{
			modelIndex = weaponViewModelID + precache_bayonet_t + 3 * knifeID + knifeIDOffset; //modelIndex = weaponViewModelID + precache_bayonet_t + 3 * knifeID + knifeIDOffset;
		}
		else if ( weaponID != itemDef || modelIndex == 0 ) { continue; }

		/* handle to viewmodel entity we will use to change the knife viewmodel index */
		DWORD knifeViewmodel = nbqmem.ReadMemory<DWORD>( csgo, localPlayer + m_hViewModel ) & 0xfff;
		DWORD knifeEntity = nbqmem.ReadMemory<DWORD>( csgo, client + dwEntityList + ( knifeViewmodel - 1 ) * 0x10 ); // ( csgo, client + m_dwEntityList + ( knifeViewmodel - 1 ) * 0x10 )
		if ( knifeEntity == 0 ) { continue; }

		/* change our current viewmodel but only if localplayer is holding a knife in hands */
		nbqmem.WriteMemory<DWORD>( csgo, knifeEntity + m_nModelIndex, modelIndex );
	}
}

namespace config
{
	std::string		m_config;
	std::string		m_current;

	auto m_exists( const std::string& name ) -> bool
	{
		std::ifstream f( name );
		return f.good();
	}

	auto m_create( const std::string& name ) -> bool
	{
		auto dir = name.substr( 0, name.find_last_of( "\\" ) ); 

		if (CreateDirectory( dir.c_str(), nullptr ) || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			std::ofstream f( name );

			if (f.good())
			{
				f.close();

				return true;
			}
		}

		return false;
	}
	auto GetPath() -> std::string
	{
		return m_config;
	}

	auto load_int(const std::string& strSection, const std::string& strName) -> int
	{
		char szData[MAX_PATH];
		GetPrivateProfileString(strSection.c_str(), strName.c_str(), "0", szData, MAX_PATH, m_current.c_str());
		return static_cast<int>(atoi(szData));
	}

	auto load_float(const std::string& strSection, const std::string& strName) -> float
	{
		char szData[MAX_PATH];
		GetPrivateProfileString(strSection.c_str(), strName.c_str(), "0", szData, MAX_PATH, m_current.c_str());
		return atof(szData);
	}

	auto load_bool(const std::string& strSection, const std::string& strName) -> bool
	{
		char szData[MAX_PATH];
		GetPrivateProfileString(strSection.c_str(), strName.c_str(), "0", szData, MAX_PATH, m_current.c_str());
		return atoi(szData);
	}

	auto save_bool(const std::string& strSection, const std::string& strName, bool bValue) -> void
	{
		WritePrivateProfileString(strSection.c_str(), strName.c_str(), std::to_string(bValue).c_str(), m_current.c_str());
	}

	auto save_int(const std::string& strSection, const std::string& strName, int iValue) -> void
	{
		WritePrivateProfileString(strSection.c_str(), strName.c_str(), std::to_string(iValue).c_str(), m_current.c_str());
	}

	auto save_float(const std::string& strSection, const std::string& strName, float flValue) -> void
	{
		WritePrivateProfileString(strSection.c_str(), strName.c_str(), std::to_string(flValue).c_str(), m_current.c_str());
	}
	auto save(const std::string& name) -> void
	{
		std::string visuals( "visuals" );
		std::string misc( "misc" );
		std::string changer( "changer" );
		std::string keys( "keys" );
		std::string offsets( "offsets" );
		auto current = name;

		if (current.find(".cfg") == std::string::npos)
			current.append(".cfg");

		m_current = m_config + current;

		if (!m_exists(m_current))
		{
			if (!m_create(m_current))
				MessageBox(GetActiveWindow(), "config created!", "", NULL);

		}
		//---visuals---//
		save_bool( visuals, "glowesp", glow );
		save_float( visuals, "GlowR", red );
		save_float( visuals, "GlowG", green );
		save_float( visuals, "GlowB", blue );
		save_float( visuals, "aGlowR", reds );
		save_float( visuals, "aGlowG", greens );
		save_float( visuals, "aGlowB", blues );
		save_int( visuals, "FOV_value", vmfov );

		//---misc---//
		save_bool( misc, "bhop", bhop );
		save_bool( misc, "rcs", norecoil );
		save_float( misc, "RCScale", skalercs );
		save_bool( misc, "triggerbot", trigger );
		save_float( misc, "triggerdelay", tbdelay );
		save_int( misc, "triggerspraydelay", tbspraydelay );
		save_bool( misc, "radarhack", radar );

		//---changer---//
		save_float( changer, "AllSkinsWear", allskinswear ); //speech 100
		save_int( changer, "KnifeModelID", kmodelid );
		save_int( changer, "KnifeSkinID", kskinid );
		save_int( changer, "deagle_skinid", deagle_skinid );
		save_int( changer, "glock_skinid", glock_skinid );
		save_int( changer, "ak47_skinid", ak47_skinid );
		save_int( changer, "awp_skinid", awp_skinid );
		save_int( changer, "m4a1s_skinid", m4a1s_skinid );
		save_int( changer, "usp_skinid", usp_skinid );
		save_int( changer, "m4a4_skinid", m4a4_skinid );
		save_int( changer, "scar20_skinid", scar20_skinid );
		save_int( changer, "scout_skinid", scout_skinid );
		save_int( changer, "aug_skinid", aug_skinid );
		save_int( changer, "revolver_skinid", revolver_skinid );
		save_int( changer, "cz75_skinid", cz75_skinid );
		save_int( changer, "p250_skinid", p250_skinid );
		save_int( changer, "sg553_skinid", sg553_skinid );
		save_int( changer, "nova_skinid", nova_skinid );
		save_int( changer, "mp9_skinid", mp9_skinid );
		save_int( changer, "mp7_skinid", mp7_skinid );
		save_int( changer, "p2000_skinid", p2000_skinid );
		save_int( changer, "tec9_skinid", tec9_skinid );
		save_int( changer, "sawedoff_skinid", sawedoff_skinid );
		save_int( changer, "negev_skinid", negev_skinid );
		save_int( changer, "mag7_skinid", mag7_skinid );
		save_int( changer, "bizon_skinid", bizon_skinid );
		save_int( changer, "xm1014_skinid", xm1014_skinid );
		save_int( changer, "ump45_skinid", ump45_skinid );
		save_int( changer, "mp5sd_skinid", mp5sd_skinid );
		save_int( changer, "p90_skinid", p90_skinid );
		save_int( changer, "mac10_skinid", mac10_skinid );
		save_int( changer, "m249_skinid", m249_skinid );
		save_int( changer, "galil_skinid", galil_skinid );
		save_int( changer, "g3sg1_skinid", g3sg1_skinid );
		save_int( changer, "famas_skinid", famas_skinid );
		save_int( changer, "fiveseven_skinid", fiveseven_skinid );
		save_int( changer, "dualberettas_skinid", dualberettas_skinid );

		//---keys---//
		save_int( keys, "bhop_key", bhopkey );
		save_int( keys, "trigger_key", triggerkey );
		save_int( keys, "disablechanger_key", disablechangerkey );
		save_int( keys, "panic_key", panickey );

		//---offsets---//
		save_int( offsets, "dwTeam_offset", dwTeam );
		save_int( offsets, "dwDormant_offset", dwDormant );
		save_int( offsets, "dwFlags_offset", dwFlags );
		save_int( offsets, "m_iShotsFired_offset", m_iShotsFired );
		save_int( offsets, "m_aimPunchAngle_offset", m_aimPunchAngle );
		save_int( offsets, "m_iCrosshairId_offset", m_iCrosshairId );
		save_int( offsets, "m_lifeState_offset", m_lifeState );
		save_int( offsets, "DwEntitySize_offset", DwEntitySize );
		save_int( offsets, "m_bSpotted_offset", m_bSpotted );
		save_int( offsets, "m_hViewModel_offset", m_hViewModel );
		save_int( offsets, "m_iViewModelIndex_offset", m_iViewModelIndex );
		save_int( offsets, "m_flFallbackWear_offset", m_flFallbackWear );
		save_int( offsets, "m_nFallbackPaintKit_offset", m_nFallbackPaintKit );
		save_int( offsets, "m_iItemIDHigh_offset", m_iItemIDHigh );
		save_int( offsets, "m_iEntityQuality_offset", m_iEntityQuality );
		save_int( offsets, "m_iItemDefinitionIndex_offset", m_iItemDefinitionIndex );
		save_int( offsets, "m_hActiveWeapon_offset", m_hActiveWeapon );
		save_int( offsets, "m_hMyWeapons_offset", m_hMyWeapons );
		save_int( offsets, "m_nModelIndex_offset", m_nModelIndex );
	}

}
 
struct glow_t
{
	DWORD dwBase;
	float r;
	float g;
	float b;
	float a;
	uint8_t unk1[16];
	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	bool m_bFullBloom;
	uint8_t unk2[14];
};

 
struct Entity
{
	DWORD dwBase;
	int team;
	bool is_dormant;
};

 
struct Player
{
	DWORD dwBase;
	bool isDormant;
};

process memory;
process _modClient;
process* mem;
PModule modClient;
PModule modEngine;

int iFriendlies;
int iEnemies;

Entity entEnemies[32];
Entity entFriendlies[32];
Entity me;

void update_entity_data(Entity* e, DWORD dwBase)
{
	int dormant = memory.Read<int>(dwBase + dwDormant);
	e->dwBase = dwBase;
	e->team = memory.Read<int>(dwBase + dwTeam);
	e->is_dormant = dormant == 1;
}
 
PModule* GetClientModule() {
	if (modClient.dwBase == 0 && modClient.dwSize == 0) {
		modClient = memory.GetModule(scale_form ? "client.dll" : "client_panorama.dll");
	}
	return &modClient;
}

PModule* GetEngineModule() {
	if (modEngine.dwBase == 0 && modEngine.dwSize == 0) {
		modEngine = memory.GetModule("engine.dll");
	}
	return &modEngine;
}

Entity* GetEntityByBase(DWORD dwBase) {

	for (int i = 0; i < iFriendlies; i++) {
		if (dwBase == entFriendlies[i].dwBase) {
			return &entFriendlies[i];
		}
	}
	for (int i = 0; i < iEnemies; i++) {
		if (dwBase == entEnemies[i].dwBase) {
			return &entEnemies[i];
		}
	}
	return nullptr;
}

class offset
{
private:
	static void update_local_player() {
		DWORD lpStart = mem->FindPatternArray(modClient.dwBase, modClient.dwSize, "xxx????xx????xxxxx?", 19, 0x8D, 0x34, 0x85, 0x0, 0x0, 0x0, 0x0, 0x89, 0x15, 0x0, 0x0, 0x0, 0x0, 0x8B, 0x41, 0x8, 0x8B, 0x48, 0x0);
		DWORD lpP1 = mem->Read<DWORD>(lpStart + 3);
		BYTE lpP2 = mem->Read<BYTE>(lpStart + 18);
		dwLocalPlayer = (lpP1 + lpP2) - modClient.dwBase;
	}

	static void update_entity_list() {
		DWORD elStart = mem->FindPatternArray(modClient.dwBase, modClient.dwSize, "x????xx?xxx", 11, 0x5, 0x0, 0x0, 0x0, 0x0, 0xC1, 0xE9, 0x0, 0x39, 0x48, 0x4);
		DWORD elP1 = mem->Read<DWORD>(elStart + 1);
		BYTE elP2 = mem->Read<BYTE>(elStart + 7);
		dwEntityList = (elP1 + elP2) - modClient.dwBase;
	}

	static void update_glow() {
		DWORD gpStart = mem->FindPatternArray(modClient.dwBase, modClient.dwSize, "xxx????xxxxx????????", 20, 0x0F, 0x11, 0x05, 0x0, 0x0, 0x0, 0x0, 0x83, 0xC8, 0x01, 0xC7, 0x05, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
		dwGlow = mem->Read<DWORD>(gpStart + 3) - modClient.dwBase;
	}

	static void update_Jump() {
		DWORD jStart = mem->FindPatternArray(modClient.dwBase, modClient.dwSize, "xx????xxxxxxx", 23, 0x8B, 0x0D, 0x0, 0x0, 0x0, 0x0, 0x8B, 0xD6, 0x8B, 0xC1, 0x83, 0xCA, 0x02);
		DWORD jOff = mem->Read<DWORD>(jStart + 2);
		dwJump = jOff - modClient.dwBase;
	}

	static void update_ViewAngles() {
		DWORD esStart = mem->FindPatternArray(modEngine.dwBase, modEngine.dwSize, "xxxx????xxxxx", 13, 0xF3, 0x0F, 0x11, 0x80, 0x0, 0x0, 0x0, 0x0, 0xD9, 0x46, 0x04, 0xD9, 0x05);
		dwClientState_ViewAngles = mem->Read<DWORD>(esStart + 4);
	}

	static void update_ClientState() {
		DWORD epStart = mem->FindPatternArray(modEngine.dwBase, modEngine.dwSize, "x????xxx?x?xxxx", 15, 0xA1, 0x0, 0x0, 0x0, 0x0, 0x33, 0xD2, 0x6A, 0x0, 0x6A, 0x0, 0x33, 0xC9, 0x89, 0xB0);
		dwClientState = mem->Read<DWORD>(epStart + 1) - modEngine.dwBase;
	}

	static void update_ClientState_state() {
		DWORD ewpStart = mem->FindPatternArray(modEngine.dwBase, modEngine.dwSize, "xx????xxxx", 11, 0x83, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0F, 0x94, 0xC0, 0xC3);
		dwClientState_state = mem->Read<DWORD>(ewpStart + 2);
	}

public:
	static void get_offset(process* m) {
		mem = m;
		modClient = mem->GetModule(scale_form ? "client.dll" : "client_panorama.dll");
		modEngine = mem->GetModule("engine.dll");
		update_local_player();
		update_entity_list();
		update_glow();
		update_Jump();
		update_ViewAngles();
		update_ClientState();
		update_ClientState_state();
	}

	 
	static DWORD WINAPI scan_offsets(LPVOID PARAM)
	{
		Entity players[64];
		while (true) {
			Sleep(1);
			DWORD playerBase = memory.Read<DWORD>(GetClientModule()->dwBase + dwLocalPlayer);
			int cp = 0;

			update_entity_data(&me, playerBase);
			for (int i = 1; i < 64; i++) {
				DWORD entBase = memory.Read<DWORD>((GetClientModule()->dwBase + dwEntityList) + i * 0x10);

				if (entBase == NULL)
					continue;

				update_entity_data(&players[cp], entBase);

				cp++;
			}

			int cf = 0, ce = 0;

			for (int i = 0; i < cp; i++) {
				if (players[i].team == me.team) {
					entFriendlies[cf] = players[i];
					cf++;
				}
				else {
					entEnemies[ce] = players[i];
					ce++;
				}
			}
			iEnemies = ce;
			iFriendlies = cf;
		}
	}
};


class virtualesp
{
private:
	static void glow_player(DWORD mObj, float r, float g, float b)
	{
		memory.Write<float>(mObj + 0x4, r);
		memory.Write<float>(mObj + 0x8, g);
		memory.Write<float>(mObj + 0xC, b);
		memory.Write<float>(mObj + 0x10, 1.0f);
		memory.Write<BOOL>(mObj + 0x24, true);
		memory.Write<BOOL>(mObj + 0x25, false);
	}

public:
	static void start_engine() {
		while (!memory.Attach("csgo.exe", PROCESS_ALL_ACCESS)) {
			Sleep(100);
		}
		do {
			Sleep(1000);
			offset::get_offset(&memory);
		} while (dwLocalPlayer < 65535);
		CreateThread(NULL, NULL, &offset::scan_offsets, NULL, NULL, NULL);
	}

	static unsigned long __stdcall esp_thread(void*)
	{
		int objectCount;
		DWORD pointerToGlow;
		Entity* Player = NULL;

		while (true)
		{
			Sleep(10);
			pointerToGlow = memory.Read<DWORD>(GetClientModule()->dwBase + dwGlow); //
			objectCount = memory.Read<DWORD>(GetClientModule()->dwBase + dwGlow + 0x4);
			if (pointerToGlow != NULL && objectCount > 0)
			{
				for (int i = 0; i < objectCount; i++)
				{
					//Sleep(1); //flicker

					DWORD mObj = pointerToGlow + i * sizeof(glow_t);
					glow_t glowObject = memory.Read<glow_t>(mObj);
					Player = GetEntityByBase(glowObject.dwBase);

					if (glowObject.dwBase == NULL || Player == nullptr || Player->is_dormant) {
						continue;
					}
					if (me.team == Player->team) {
						glow_player(mObj, reds, greens, blues);  
					}
					else {
						glow_player(mObj, red, green, blue);  
					}
				}
			}
		}
		return EXIT_SUCCESS;
	}
};

DWORD GetEnginePointer()
{
	return mem->Read<DWORD>(modEngine.dwBase + dwClientState);
}

bool IsInGame()
{
	if (mem->Read<int>(GetEnginePointer() + dwClientState_state) == 6)
	{
		return true;
	}
	return false;
}

Vector GetViewAngles()
{
	return mem->Read<Vector>(GetEnginePointer() + dwClientState_ViewAngles);
}

void SetViewAngles(Vector angles)
{
	mem->Write<Vector>(GetEnginePointer() + dwClientState_ViewAngles, angles);
}

DWORD GetLocalPlayer()
{
	return mem->Read<DWORD>(modClient.dwBase + dwLocalPlayer);
}

int GetShotsFired()
{
	return mem->Read<int>(GetLocalPlayer() + m_iShotsFired);
}

int getLocalTeam()
{
	return mem->Read<int>(GetLocalPlayer() + dwTeam);
}

int GetCrosshairId()
{
	DWORD PlayerBase = GetLocalPlayer();
	if (PlayerBase)
	{
		return mem->Read<int>(PlayerBase + m_iCrosshairId) - 1;
	}
}

DWORD GetBaseEntity(int PlayerNumber)
{
	return mem->Read<DWORD>(modClient.dwBase + dwEntityList + (DwEntitySize * PlayerNumber));
}

int GetTeam(int PlayerNumber)
{
	DWORD BaseEntity = GetBaseEntity(PlayerNumber);
	if (BaseEntity)
	{
		return mem->Read<int>(BaseEntity + dwTeam);
	}
}

int GetTeam()
{
	DWORD PlayerBase = GetLocalPlayer();
	if (PlayerBase)
	{
		return mem->Read<int>(PlayerBase + dwTeam);
	}
}

bool IsDead(int PlayerNumber)
{
	DWORD BaseEntity = GetBaseEntity(PlayerNumber);
	if (BaseEntity)
	{
		return mem->Read<bool>(BaseEntity + m_lifeState);
	}
}

class cheats
{
public:
	static unsigned long __stdcall vmfov_thread( void* )
	{
		while ( true )
		{
			int LocalBase = memory.Read<int>( GetClientModule()->dwBase + dwLocalPlayer );
			memory.Write<int>( LocalBase + 0x330C, vmfov );
		}
	}

	static unsigned long __stdcall radar_thread( void* )
	{
		while ( true )
		{
			for ( int i = 1; i <= 64; i++ )
			{
				DWORD dwEnemy = memory.Read<DWORD>( ( GetClientModule()->dwBase + dwEntityList ) + i * 0x10 );
				memory.Write<int>( dwEnemy + m_bSpotted, 1 );
				Sleep( 1 );
			}
			Sleep( 1 );
		}
		return EXIT_SUCCESS;
	}
	static unsigned long __stdcall bhop_thread(void*)
	{
		while (true)
		{
			DWORD dwPla = memory.Read<DWORD>(GetClientModule()->dwBase + dwLocalPlayer);
			int flags = memory.Read<DWORD>(dwPla + dwFlags);
			
			if ((GetAsyncKeyState(bhopkey) & 0x8000) && ( flags & 0x1 == 1 ) ) {
				memory.Write<int>(GetClientModule()->dwBase + dwJump, 5);
				Sleep(25);
				memory.Write<int>(GetClientModule()->dwBase + dwJump, 4);
			}
			Sleep(1);
		}
		return EXIT_SUCCESS;
	}

	static unsigned long __stdcall norecoil_thread(void*)
	{
		Vector viewAngle;
		Vector punchAngle;
		Vector oldAngle;
		int rcsScale = skalercs;

		while (true)
		{
			if (IsInGame())
			{
				viewAngle = GetViewAngles();
				punchAngle = mem->Read<Vector>(GetLocalPlayer() + m_aimPunchAngle);

				if (GetShotsFired() > 1)
				{
					viewAngle.x -= (punchAngle.x - oldAngle.x) * (rcsScale * 0.02f);
					viewAngle.y -= (punchAngle.y - oldAngle.y) * (rcsScale * 0.02f);
					ClampVector(viewAngle);
					SetViewAngles(viewAngle);
					oldAngle.x = punchAngle.x;
					oldAngle.y = punchAngle.y;
				}
				else
				{
					oldAngle.x = punchAngle.x;
					oldAngle.y = punchAngle.y;
				}
			}
			Sleep(1);
		}
		return EXIT_SUCCESS;
	}

	static unsigned long __stdcall trigger_thread(void*)
	{
		while (true)
		{
			if (GetAsyncKeyState( triggerkey ) < 0 && triggerkey != 1)
			{
				int PlayerNumber = GetCrosshairId();
				if (PlayerNumber < 64 && PlayerNumber >= 0 && GetTeam(PlayerNumber) != GetTeam() && IsDead(PlayerNumber) != true)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
					Sleep( tbspraydelay );
					mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
					Sleep( tbspraydelay );
				}
				else
				{
					Sleep(tbdelay);
				}
			}
			Sleep(1);
		}
		return EXIT_SUCCESS;
	}
};

HANDLE ESP = NULL;
HANDLE BHOP = NULL;
HANDLE NORECOIL = NULL;
HANDLE TRIGGER = NULL;
HANDLE RADAR = NULL;
HANDLE VMFOV = NULL;
//HANDLE CHAMS = NULL;
using namespace config;
auto load(const std::string& name) -> void
{
	std::string visuals("visuals");
	std::string misc( "misc" );
	std::string changer( "changer" );
	std::string keys( "keys" );
	std::string offsets( "offsets" );
	auto current = name;

	if (current.find(".cfg") == std::string::npos) // ок
		current.append(".cfg");

	m_current = m_config + current;

	if (!m_exists(m_current))
		save(name);

	//---visuals---//
	glow = load_bool( visuals, "glowesp" );
	red = load_float( visuals, "GlowR" );
	green = load_float( visuals, "GlowG" );
	blue = load_float( visuals, "GlowB" );
	reds = load_float( visuals, "aGlowR" );
	greens = load_float( visuals, "aGlowG" );
	blues = load_float( visuals, "aGlowB" );
	vmfov = load_int( visuals, "FOV_value" );
	//---misc---//
	bhop = load_bool( misc, "bhop" );
	norecoil = load_bool( misc, "rcs" );
	skalercs = load_float( misc, "RCScale" );
	trigger = load_bool( misc, "triggerbot" );
	tbdelay = load_float( misc, "triggerdelay" );
	tbspraydelay = load_int( misc, "triggerspraydelay" );
	radar = load_bool( misc, "radarhack" );
	//---changer---//
	allskinswear = load_float( changer, "AllSkinsWear" );
	kmodelid = load_int( changer, "KnifeModelID" );
	kskinid = load_int( changer, "KnifeSkinID" );
	deagle_skinid = load_int( changer, "deagle_skinid" );
	glock_skinid = load_int( changer, "glock_skinid" );
	ak47_skinid = load_int( changer, "ak47_skinid" );
	awp_skinid = load_int( changer, "awp_skinid" );
	m4a1s_skinid = load_int( changer, "m4a1s_skinid" );
	usp_skinid = load_int( changer, "usp_skinid" );
	m4a4_skinid = load_int( changer, "m4a4_skinid" );
	scar20_skinid = load_int( changer, "scar20_skinid" );
	scout_skinid = load_int( changer, "scout_skinid" );
	aug_skinid = load_int( changer, "aug_skinid" );
	revolver_skinid = load_int( changer, "revolver_skinid" );
	cz75_skinid = load_int( changer, "cz75_skinid" );
	p250_skinid = load_int( changer, "p250_skinid" );
	sg553_skinid = load_int( changer, "sg553_skinid" );
	nova_skinid = load_int( changer, "nova_skinid" );
	mp9_skinid = load_int( changer, "mp9_skinid" );
	mp7_skinid = load_int( changer, "mp7_skinid" );
	p2000_skinid = load_int( changer, "p2000_skinid" );
	tec9_skinid = load_int( changer, "tec9_skinid" );
	sawedoff_skinid = load_int( changer, "sawedoff_skinid" );
	negev_skinid = load_int( changer, "negev_skinid" );
	mag7_skinid = load_int( changer, "mag7_skinid" );
	bizon_skinid = load_int( changer, "bizon_skinid" );
	xm1014_skinid = load_int( changer, "xm1014_skinid" );
	ump45_skinid = load_int( changer, "ump45_skinid" );
	mp5sd_skinid = load_int( changer, "mp5sd_skinid" );
	p90_skinid = load_int( changer, "p90_skinid" );
	mac10_skinid = load_int( changer, "mac10_skinid" );
	m249_skinid = load_int( changer, "m249_skinid" );
	galil_skinid = load_int( changer, "galil_skinid" );
	g3sg1_skinid = load_int( changer, "g3sg1_skinid" );
	famas_skinid = load_int( changer, "famas_skinid" );
	fiveseven_skinid = load_int( changer, "fiveseven_skinid" );
	dualberettas_skinid = load_int( changer, "dualberettas_skinid" );
	//---keys---//
	bhopkey = load_int( keys, "bhop_key" );
	triggerkey = load_int( keys, "trigger_key" );
	disablechangerkey = load_int( keys, "disablechanger_key" );
	panickey = load_int( keys, "panic_key" );
	//---offsets---//
	dwTeam = load_int( offsets, "dwTeam_offset" );
	dwDormant = load_int( offsets, "dwDormant_offset" );
	dwFlags = load_int( offsets, "dwFlags_offset" );
	m_iShotsFired = load_int( offsets, "m_iShotsFired_offset" );
	m_aimPunchAngle = load_int( offsets, "m_aimPunchAngle_offset" );
	m_iCrosshairId = load_int( offsets, "m_iCrosshairId_offset" );
	m_lifeState = load_int( offsets, "m_lifeState_offset" );
	DwEntitySize = load_int( offsets, "DwEntitySize_offset" );
	m_bSpotted = load_int( offsets, "m_bSpotted_offset" );
	m_hViewModel = load_int( offsets, "m_hViewModel_offset" );
	m_iViewModelIndex = load_int( offsets, "m_iViewModelIndex_offset" );
	m_flFallbackWear = load_int( offsets, "m_flFallbackWear_offset" );
	m_nFallbackPaintKit = load_int( offsets, "m_nFallbackPaintKit_offset" );
	m_iItemIDHigh = load_int( offsets, "m_iItemIDHigh_offset" );
	m_iEntityQuality = load_int( offsets, "m_iEntityQuality_offset" );
	m_iItemDefinitionIndex = load_int( offsets, "m_iItemDefinitionIndex_offset" );
	m_hActiveWeapon = load_int( offsets, "m_hActiveWeapon_offset" );
	m_hMyWeapons = load_int( offsets, "m_hMyWeapons_offset" );
	m_nModelIndex = load_int( offsets, "m_nModelIndex_offset" );

	if (glow)
		ESP = CreateThread(NULL, NULL, &virtualesp::esp_thread, NULL, NULL, NULL);


	if (bhop)
		BHOP = CreateThread(NULL, NULL, &cheats::bhop_thread, NULL, NULL, NULL);


	if (norecoil)
		NORECOIL = CreateThread(NULL, NULL, &cheats::norecoil_thread, NULL, NULL, NULL);


	if (trigger)
		TRIGGER = CreateThread(NULL, NULL, &cheats::trigger_thread, NULL, NULL, NULL);


	if ( radar )
		RADAR = CreateThread( NULL, NULL, &cheats::radar_thread, NULL, NULL, NULL );


	if ( vmfov )
		VMFOV = CreateThread( NULL, NULL, &cheats::vmfov_thread, NULL, NULL, NULL );


	//if ( chams )
	//	CHAMS = CreateThread( NULL, NULL, &cheats::chams_player, NULL, NULL, NULL );
}

DWORD users[] =
{
	4067806540,
	41535429
};

static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int w = 250, h = 170; // w за лево, h в низ w = 314, h = 404
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL WINAPI WinMain( HINSTANCE, HINSTANCE, CHAR*, INT )
{
	{
		MessageBox( 0, "note: close Steam before starting the cheat", "", MB_OK | MB_ICONINFORMATION );

	}
	config::m_config = "\\"; 
	static bool m_status = false;
	ULONG id = 0;
	GetVolumeInformationA( "C:\\", NULL, NULL, &id, NULL, NULL, NULL, NULL );
	for ( auto i = 0; i < sizeof( users ) / sizeof( *users ); i++ )
	{
		if ( id == users[ i ] )
			m_status = true;
	}

	if ( !m_status )
	{
		MessageBox( GetActiveWindow(), "hwid denied", "0", NULL );
		exit( NULL );
	}


	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle( NULL ), NULL, LoadCursor( NULL, IDC_ARROW ), NULL, NULL, _T( "didisko" ), NULL };
	RegisterClassEx( &wc );
	HWND hwnd = CreateWindow( _T( "didisko" ), _T( "iextern private" ), WS_POPUP, 255, 225, w, h, NULL, NULL, wc.hInstance, NULL );


	LPDIRECT3D9 pD3D;
	if ( ( pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
	{
		UnregisterClass( _T( "didisko" ), wc.hInstance );
		return 0;
	}
	ZeroMemory( &g_d3dpp, sizeof( g_d3dpp ) );
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if ( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) < 0 )
	{
		pD3D->Release();
		UnregisterClass( _T( "didisko" ), wc.hInstance );
		return 0;
	}

	ImGui_ImplDX9_Init( hwnd, g_pd3dDevice );


	bool show_test_window = false;
	bool show_another_window = false;
	bool m_menu_active = false;
	ImVec4 clear_color = ImVec4( 0.51f, 0.51f, 0.61f, 1.0f );

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	ShowWindow( hwnd, SW_SHOWDEFAULT );
	UpdateWindow( hwnd );





	while ( msg.message != WM_QUIT )
	{
		if ( GetAsyncKeyState( panickey ) & 1 )
		{
			TerminateThread( ESP, 0 );
			CloseHandle( ESP );
			TerminateThread( BHOP, 0 );
			CloseHandle( BHOP );
			TerminateThread( RADAR, 0 );
			CloseHandle( RADAR );
			TerminateThread( VMFOV, 0 );
			CloseHandle( VMFOV );
			TerminateThread( NORECOIL, 0 );
			CloseHandle( NORECOIL );
			TerminateThread( TRIGGER, 0 );
			CloseHandle( TRIGGER );
			g_pd3dDevice->EndScene();
			ImGui_ImplDX9_Shutdown(); // somewhat harakiri
		}

		if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}

		ImGui_ImplDX9_NewFrame();

		auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_ShowBorders;

		ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
		static bool kek = false;
		static bool glow_check = false, bhop_check = false, trigger_check = false, rcs_check = false, radar_check = false, gg = false, vmfov_check = false;

		ImGui::Begin( "iextern private", nullptr, ImVec2( w, h ), 0.4f, flags );
		{
			//std::string user_name = "seniusz";
			//ImGui::Text( "welcome, agent %s! :)", user_name.data() );
			ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "build date 27.11.18" );
			ImGui::SameLine( 220 );
			if ( ImGui::Button( " - ", ImVec2( 0, 15 ) ) )
				exit( 0 );

			std::string correct_pass = "7890";
			
			static char password[ 5 ] = "";
			if ( password != correct_pass )
			{
				ImGui::BeginChild( " auth step ", ImVec2( 230, 110 ), false, ImGuiWindowFlags_SenyaPidoras ); // 155, 67
				ImGui::PushItemWidth( 37 );
				ImGui::SetCursorPos( ImVec2( 12, 19 ) ); // 1 number - weight сторона, 2 number - height низ.
				ImGui::InputText( "enter your key", password, 5 );
				ImGui::EndChild();
			}
			//ImGui::BeginChild(u8" Контакты ", ImVec2(185, 100), false, ImGuiWindowFlags_SenyaPidoras);
			//ImGui::Text(" ");
			//ImGui::Text("  VK: id/seniusz/");
			//ImGui::Text("  Steam: id/seniusz/");
			//ImGui::Text("  Discord: seniusz#5219");
			//ImGui::EndChild();
			//ImGui::Separator();

			auto red_color = ImColor( 255, 0, 0, 0 );
			auto green_color = ImColor( 0, 255, 0 );
			auto pink_color = ImColor( 255, 0, 255 );
			auto blue_color = ImColor( 0, 0, 255 );

			if ( password == correct_pass )
			{

				if ( !ssd )
				{

					if ( !FindWindowA( NULL, "Counter-Strike: Global Offensive" ) )
					{
						{
							Sleep( 450 );
							MessageBox( 0, "you should run cs:go first", "error", MB_OK );
						}
						return 0;
						//ImGui::Text( u8"Ожидание запуска CS:GO ..." );
						//const ImU32 col = ImGui::GetColorU32( ImGuiCol_ButtonHovered );
						//ImGui::Spinner( "##spinner", 15, 6, col );
						//ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Waiting for CS:GO to launch ...");
					}
					else
					{
						{
							ImGui::BeginChild( " module step ", ImVec2( 230, 110 ), false, ImGuiWindowFlags_SenyaPidoras );

							ImGui::SetCursorPos( ImVec2( 6, 6 ) ); // 1 number - weight сторона, 2 number - height низ.
							ImGui::TextColored( ImVec4( 1.f, 1.f, 1.f, 1.f ), " cs:go module is ..." );
							ImGui::SetCursorPos( ImVec2( 9, 27 ) ); // 1 number - weight сторона, 2 number - height низ.
							if ( ImGui::Button( "client", ImVec2( 44, 0 ) ) )
							{
								scale_form = true;
								Sleep( 1 );
								ssd = true;
							}

							ImGui::SameLine();
							
							if ( ImGui::Button( "client_panorama", ImVec2( 118, 0 ) ) )
								ssd = true;
							ImGui::EndChild();
						}
					}
				}
				else
				{					
					ImGui::BeginChild( " main ", ImVec2( 230, 110 ), false, ImGuiWindowFlags_SenyaPidoras );
					ImGui::SetCursorPos( ImVec2( 50, 11 ) ); // 1 number - weight сторона, 2 number - height низ.
						if ( ImGui::Button( "  enable changer  ", ImVec2( 129, 27 ) ) )
						{
							NBQMemory mem_;
							auto m_id = mem_.GetProcessIdByProcessName( "csgo.exe" );
							auto m_handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, m_id );
							auto dwClient = mem_.GetModuleBaseAddress( m_id, scale_form ? "client.dll" : "client_panorama.dll" );
							if ( m_handle != INVALID_HANDLE_VALUE )
								skinsX( m_handle, dwClient, kmodelid, kskinid );
						}
							ImGui::SetCursorPos( ImVec2( 35, 44 ) ); // 1 number - weight сторона, 2 number - height низ.
							if ( ImGui::Button( "reload CFG", ImVec2( 81, 0 ) ) )
								load( "iextern.cfg" );

							ImGui::SameLine();

							if ( ImGui::Button( "save CFG", ImVec2( 69, 0 ) ) )
								save( "iextern.cfg" );
							ImGui::SetCursorPos( ImVec2( 17, 88 ) ); // 1 number - weight сторона, 2 number - height низ.
							ImGui::Text( "the cfg saves on Windows Disk" );
							ImGui::SetCursorPos( ImVec2( 8, 73 ) ); // 1 number - weight сторона, 2 number - height низ.
							ImGui::Text( "cheat is configured in the CFG file" );
							ImGui::EndChild();
					//static int switchTabs = 4;
					//if ( ImGui::Button( "Assists", ImVec2( 82.0f, 30.0f ) ) )
					//	switchTabs = 0;
					//ImGui::SameLine( 0.0, 2.4f );
					//if ( ImGui::Button( "Visuals", ImVec2( 82.0f, 30.0f) ) )
					//	switchTabs = 1;
					//ImGui::SameLine( 0.0, 2.4f );
					//if ( ImGui::Button( "Misc", ImVec2( 82.0f, 30.0f ) ) )
					//	switchTabs = 2;
					//ImGui::SameLine( 0.0, 2.4f );
					//if ( ImGui::Button( "Config", ImVec2( 82.0f, 30.0f ) ) )
					//	switchTabs = 3;
					//switch ( switchTabs )
					//{
					//case 0:
					//{
					//	ImGui::Separator();
					//	if ( ImGui::Checkbox( u8"Trigger Bot (L.ALT дефолт)", &trigger ) )
					//		trigger_check = !trigger_check;

					//	ImGui::Text( u8"Задержка перед выстрелом" );
					//	ImGui::PushItemWidth( 217 );
					//	ImGui::SliderFloat( "##testing8", &tbdelay, 0.f, 500.f, "%.f", 1.f );
					//	ImGui::NextColumn();

					//	ImGui::Text( u8"Задержка перед следующим спреем" );
					//	ImGui::PushItemWidth( 217 );
					//	ImGui::SliderFloat( "##testing9", &tbspraydelay, 0.f, 500.f, "%.f", 1.f );
					//	ImGui::NextColumn();
					//	ImGui::Separator();
					//	if ( ImGui::Checkbox( "Recoil Control System (RCS)", &norecoil ) )
					//		rcs_check = !rcs_check;

					//	ImGui::Text( u8"Объем RCS" );
					//	ImGui::PushItemWidth( 217 );
					//	ImGui::SliderFloat( "##testing7", &skalercs, 0.f, 100.f, "%.f", 1.f );
					//	ImGui::Text( u8"Сначала выставите значение, потом включите." );
					//	break;
					//}
					//case 1:
					//{
					//	ImGui::Separator();
					//	if ( ImGui::Checkbox( "Glow ESP", &glow ) )
					//		glow_check = !glow_check;
					//	ImGui::Text( u8"Цвет подсветки противников (RGB)" );
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing1", &red, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::SameLine();
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing2", &green, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::SameLine();
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing3", &blue, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::Text( u8"Цвет подсветки союзников (RGB)" );
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing4", &reds, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::SameLine();
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing5", &greens, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::SameLine();
					//	ImGui::PushItemWidth( 100 );
					//	ImGui::SliderFloat( "##testing6", &blues, 0.f, 1.f, "%.3f", 1.f );
					//	ImGui::Text( " " );

					//	break;
					//}
					//case 2:
					//{
					//	ImGui::Separator();
					//	ImGui::Text( u8"Ченджер настраивается через CFG чита." );
					//	ImGui::Text( u8"При нажатии меню зависнет, скины появятся." );
					//	if ( ImGui::Button( u8" Включить Changer " ) )
					//	{
					//		NBQMemory mem_;
					//		auto m_id = mem_.GetProcessIdByProcessName( "csgo.exe" );
					//		auto m_handle = OpenProcess( PROCESS_ALL_ACCESS, FALSE, m_id );
					//		auto dwClient = mem_.GetModuleBaseAddress( m_id, scale_form ? "client.dll" : "client_panorama.dll" );
					//		if ( m_handle != INVALID_HANDLE_VALUE )
					//			skinsX( m_handle, dwClient, kmodelid, kskinid );
					//	}
					//	ImGui::Text( u8"F9 (дефолт) отключит ченджер, меню отвиснет." );
					//	ImGui::Separator();
					//	if ( ImGui::Checkbox( "Bunny Hop", &bhop ) )
					//		bhop_check = !bhop_check;
					//	ImGui::TextColored( ImVec4( 1.f, 1.f, 1.f, 0.53f ), u8"                              включается в CFG файле" );
					//	ImGui::SameLine( 8 );
					//	if ( ImGui::Checkbox( "RADAR Hack", &fake ) )
					//		gg = !gg;
					//	ImGui::Text( " " );
					//	break;
					//}
					//case 3:
					//{
					//	ImGui::Separator();
					//	ImGui::Text( u8"Путь к CFG чита: C:/iextern_cheat/iext.cfg" );
					//	
					//	if ( ImGui::Button( u8"Загрузить CFG", ImVec2( 110, 0 ) ) )
					//		load( "iext.cfg" );

					//	ImGui::SameLine();

					//	if ( ImGui::Button( u8"Сохранить CFG", ImVec2( 110, 0 ) ) )
					//		save( "iext.cfg" );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8" " );
					//	ImGui::Text( u8"Группа в ВК: vk.com/iexterncheats" );
					//	break;
				

					}

				}

			}
			ImGui::End();

			if ( ssd && !kek )
			{
				virtualesp::start_engine();
				kek = true;
			}

			if ( glow_check )
			{
				if ( glow )
				{
					ESP = CreateThread( NULL, NULL, &virtualesp::esp_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( ESP, 0 );
					CloseHandle( ESP );
					Beep( 250, 150 );
				}
				glow_check = false;
			}

			if ( bhop_check )
			{
				if ( bhop )
				{
					BHOP = CreateThread( NULL, NULL, &cheats::bhop_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( BHOP, 0 );
					CloseHandle( BHOP );
					Beep( 250, 150 );
				}
				bhop_check = false;
			}

			if ( radar_check )
			{
				if ( radar )
				{
					RADAR = CreateThread( NULL, NULL, &cheats::radar_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( RADAR, 0 );
					CloseHandle( RADAR );
					Beep( 250, 150 );
				}
				radar_check = false;
			}

			if ( vmfov_check )
			{
				if ( vmfov )
				{
					VMFOV = CreateThread( NULL, NULL, &cheats::vmfov_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( VMFOV, 0 );
					CloseHandle( VMFOV );
					Beep( 250, 150 );
				}
				vmfov_check = false;
			}

			if ( rcs_check )
			{
				if ( norecoil )
				{
					NORECOIL = CreateThread( NULL, NULL, &cheats::norecoil_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( NORECOIL, 0 );
					CloseHandle( NORECOIL );
					Beep( 250, 150 );
				}
				rcs_check = false;
			}

			if ( trigger_check )
			{
				if ( trigger )
				{
					TRIGGER = CreateThread( NULL, NULL, &cheats::trigger_thread, NULL, NULL, NULL );
					Beep( 450, 150 );
				}
				else
				{
					TerminateThread( TRIGGER, 0 );
					CloseHandle( TRIGGER );
					Beep( 250, 150 );
				}
				trigger_check = false;
			}

			g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, false );
			g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
			g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA( ( int ) ( clear_color.x*255.0f ), ( int ) ( clear_color.y*255.0f ), ( int ) ( clear_color.z*255.0f ), ( int ) ( clear_color.w*255.0f ) );
			g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
			if ( g_pd3dDevice->BeginScene() >= 0 )
			{
				ImGui::Render();
				g_pd3dDevice->EndScene();
			}
			g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
		}
		ImGui_ImplDX9_Shutdown();

		if ( g_pd3dDevice )
			g_pd3dDevice->Release();
		if ( pD3D )
			pD3D->Release();
	}

