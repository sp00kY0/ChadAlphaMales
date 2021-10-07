#include "ClientHook.h"

#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/AttributeChanger/AttributeChanger.h"
#include "../../Features/RichPresence/RichPresence.h"
#include "../../SDK/Includes/bitbuf.h"
void __stdcall ClientHook::PreEntity::Hook(char const* szMapName)
{
	Table.Original<fn>(index)(g_Interfaces.Client, szMapName);
}

void __stdcall ClientHook::PostEntity::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);

	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_maxdlights 69420"));
	g_Interfaces.Engine->ClientCmd_Unrestricted(_("r_dynamic 1"));
	g_Visuals.ModulateWorld();
}

void __stdcall ClientHook::ShutDown::Hook()
{
	Table.Original<fn>(index)(g_Interfaces.Client);
	g_EntityCache.Clear();
}

void __stdcall ClientHook::FrameStageNotify::Hook(EClientFrameStage FrameStage)
{
	switch (FrameStage)
	{
		case EClientFrameStage::FRAME_RENDER_START:
		{
			g_GlobalInfo.m_vPunchAngles = Vec3();

			if (Vars::Visuals::RemovePunch.m_Var) {
				if (const auto& pLocal = g_EntityCache.m_pLocal) {
					g_GlobalInfo.m_vPunchAngles = pLocal->GetPunchAngles();	//Store punch angles to be compesnsated for in aim
					pLocal->ClearPunchAngle();								//Clear punch angles for visual no-recoil
				}
			}
			g_Visuals.ViewmodelXYZ();
			g_Visuals.ARatio();
			g_Visuals.ThirdPerson();
			g_Visuals.SkyboxChanger();
			break;
		}

		default: break;
	}
	
	Table.Original<fn>(index)(g_Interfaces.Client, FrameStage);

	if (Vars::Misc::SteamRPC.m_Var)
		source::features::steamrichpresence.Update();

	switch (FrameStage)
	{
		case EClientFrameStage::FRAME_NET_UPDATE_START: {
			g_EntityCache.Clear();
			break;
		}

//#ifdef DEVELOPER_BUILD
		case EClientFrameStage::FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
			g_AttributeChanger.Run();
			break;
		}
//#endif

		case EClientFrameStage::FRAME_NET_UPDATE_END: 
		{
			g_EntityCache.Fill();

			g_GlobalInfo.m_bLocalSpectated = false;

			if (const auto &pLocal = g_EntityCache.m_pLocal)
			{
				for (const auto &Teammate : g_EntityCache.GetGroup(EGroupType::PLAYERS_TEAMMATES))
				{
					if (Teammate->IsAlive() || g_EntityCache.Friends[Teammate->GetIndex()])
						continue;

					CBaseEntity *pObservedPlayer = g_Interfaces.EntityList->GetClientEntityFromHandle(Teammate->GetObserverTarget());

					if (pObservedPlayer == pLocal)
					{
						switch (Teammate->GetObserverMode()) {
							case OBS_MODE_FIRSTPERSON: break;
							case OBS_MODE_THIRDPERSON: break;
							default: continue;
						}

						g_GlobalInfo.m_bLocalSpectated = true;
						break;
					}
				}
			}

			break;
		}

		case EClientFrameStage::FRAME_RENDER_START: {
			g_Visuals.UpdateWorldModulation();
			break;
		}

		default: break;
	}
}

bool __stdcall ClientHook::DispatchUserMessage::Hook(int msg_type, bf_read& msg_data)
{
	/*bf_read msg_copy = msg_data;
	msg_copy.Seek(0);
	switch (msg_type) {
	case 3:
	case 4: 
		{
			char msg_name[50], playerName[128] params1 , msg[127] params2 ;


			int ent_idx = msg_copy.ReadByte();
			BOOL chat = msg_copy.ReadShort();
			msg_copy.ReadString(msg_name, 50);
			msg_copy.ReadString(playerName, 128);
			msg_copy.ReadString(msg, 127);
			BOOL textallchat = msg_copy.ReadShort();

			//std::cout << "msg_name: " << msg_name << std::endl;
			//std::cout << "player_name: " << playerName << std::endl;
			//std::cout << "message: " << msg << std::endl;
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), msg_type);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), ent_idx);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_type: %d\n"), chat);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg_name: %s\n"), msg_name);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("playerName: %s\n"), playerName);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("msg: %s\n"), msg);
			g_Interfaces.CVars->ConsoleColorPrintf({ 0, 255, 12, 255 }, _("textallchat: %d\n"), textallchat);
			break;
		}
	}*/
	return Table.Original<fn>(index)(g_Interfaces.Client, msg_type, msg_data);
}
