#pragma once
#include "../../Includes/Includes.h"

struct kbutton_t;
struct ButtonCode_t;
struct GameActionSet_t;
struct CameraThirdData_t;
struct GameActionSetFlags_t;
struct C_BaseCombatWeapon;
using QAngle = Vec3;

class IInput
{
public:
	virtual	void			Init_All(void) = 0;
	virtual void			Shutdown_All(void) = 0;
	virtual int				GetButtonBits(int) = 0;
	virtual void			CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void			ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool			WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand) = 0;
	virtual void			EncodeUserCmdToBuffer(bf_write &buf, int slot) = 0;
	virtual void			DecodeUserCmdFromBuffer(bf_read &buf, int slot) = 0;
	virtual CUserCmd		*GetUserCmd(int sequence_number) = 0;
	virtual void			MakeWeaponSelection(C_BaseCombatWeapon *weapon) = 0;
	virtual float			KeyState(kbutton_t *key) = 0;
	virtual int				KeyEvent(int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding) = 0;
	virtual kbutton_t		*FindKey(const char *name) = 0;
	virtual void			ControllerCommands(void) = 0;
	virtual void			Joystick_Advanced(void) = 0;
	virtual void			Joystick_SetSampleTime(float frametime) = 0;
	virtual void			IN_SetSampleTime(float frametime) = 0;
	virtual void			AccumulateMouse(void) = 0;
	virtual void			ActivateMouse(void) = 0;
	virtual void			DeactivateMouse(void) = 0;
	virtual void			ClearStates(void) = 0;
	virtual float			GetLookSpring(void) = 0;
	virtual void			GetFullscreenMousePos(int *mx, int *my, int *unclampedx = 0, int *unclampedy = 0) = 0;
	virtual void			SetFullscreenMousePos(int mx, int my) = 0;
	virtual	float			GetLastForwardMove(void) = 0;
	virtual	float			Joystick_GetForward(void) = 0;
	virtual	float			Joystick_GetSide(void) = 0;
	virtual	float			Joystick_GetPitch(void) = 0;
	virtual	float			Joystick_GetYaw(void) = 0;
	virtual void			CAM_Think(void) = 0;
	virtual int				CAM_IsThirdPerson(void) = 0;
	virtual void			CAM_ToThirdPerson(void) = 0;
	virtual void			CAM_ToFirstPerson(void) = 0;
	virtual void			CAM_StartMouseMove(void) = 0;
	virtual void			CAM_EndMouseMove(void) = 0;
	virtual void			CAM_StartDistance(void) = 0;
	virtual void			CAM_EndDistance(void) = 0;
	virtual int				CAM_InterceptingMouse(void) = 0;
	virtual void			CAM_ToOrthographic() = 0;
	virtual	bool			CAM_IsOrthographic() const = 0;
	virtual	void			CAM_OrthographicSize(float &w, float &h) const = 0;
	virtual void			SetPreferredGameActionSet(GameActionSet_t action_set) = 0;
	virtual GameActionSet_t GetPreferredGameActionSet() = 0;
	virtual void			SetGameActionSetFlags(GameActionSetFlags_t action_set_flags) = 0;
	virtual void			LevelInit(void) = 0;
	virtual void			ClearInputButton(int bits) = 0;
	virtual	void			CAM_SetCameraThirdData(CameraThirdData_t *pCameraData, const QAngle &vecCameraOffset) = 0;
	virtual void			CAM_CameraThirdThink(void) = 0;
	virtual	bool			EnableJoystickMode() = 0;
	virtual bool			IsSteamControllerActive() = 0;
};