#pragma once

#include "../BaseEntity/BaseEntity.h"

#define MAX_NEW_COMMANDS 15
#define MAX_NEW_COMMANDS_HEAVY 24
#define DT_WAIT_CALLS 24
#define MULTIPLAYER_BACKUP 90

struct GlobalInfo_t
{
	int m_net_sendto					= 0;
	int m_nCurrentTargetIdx				= 0;
	int m_nCurItemDefIndex              = 0;
	int m_nWaitForShift                 = 0;
	int m_nShifted                      = MAX_NEW_COMMANDS;
	float m_Latency						= 0;
	bool m_bWeaponCanHeadShot			= false;
	bool m_bWeaponCanAttack				= false;
	bool m_bWeaponCanSecondaryAttack	= false;
	bool m_bAAActive					= false;
	bool m_bHitscanSilentActive			= false;
	bool m_bProjectileSilentActive		= false; //flamethrower
	bool m_bAutoBackstabRunning			= false;
	bool m_bHitscanRunning				= false;
	bool m_bSilentTime					= false;
	bool m_bLocalSpectated				= false;
	bool m_bRollExploiting			    = false;
	bool m_bAttacking			 	    = false;
	bool m_bModulateWorld				= true;
	bool m_bShouldShift                 = false;
	bool m_bRecharging                  = false;
	bool fast_stop						= false;
	bool Unload							= false;
	float m_flCurAimFOV					= 0.0f;
	Vec3 m_vPredictedPos				= {};
	Vec3 m_vAimPos						= {};
	VMatrix m_WorldToProjection			= {};
	Vec3 m_vViewAngles					= {};
	Vec3 m_vRealViewAngles				= {};
	Vec3 m_vFakeViewAngles				= {};
	Vec3 m_vPunchAngles					= {};
	EWeaponType m_WeaponType			= {};
	std::vector<int> storedmis			{ 0 };
};

inline GlobalInfo_t g_GlobalInfo;