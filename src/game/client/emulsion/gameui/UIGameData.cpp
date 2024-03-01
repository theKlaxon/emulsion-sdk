#include "cbase.h"
#include "UIGameData.h"

#include "gameui_util.h"
#include "steam/steam_api.h"

namespace BaseModUI
{
	CUIGameData* CUIGameData::m_Singleton = 0;
	bool CUIGameData::m_bModuleShutDown = false;

	CUIGameData* CUIGameData::Get()
	{
		if (!m_Singleton && !m_bModuleShutDown)
			m_Singleton = new CUIGameData();

		return m_Singleton;
	}

	void CUIGameData::ShutDown()
	{
		delete m_Singleton;
		m_Singleton = NULL;
		m_bModuleShutDown = true;
	}

	void CUIGameData::RunFrame() {

	}

	void CUIGameData::OnGameUIPostInit()
	{
	}

	void CUIGameData::NeedConnectionProblemWaitScreen()
	{
	}

	void CUIGameData::ShowPasswordUI(char const*pchCurrentPW)
	{
	}

	// Klaxnote - I dont rlly have time to disect this in asw or implement it. sorry.
	char const* CUIGameData::GetPlayerName(XUID playerID, char const* szPlayerNameSpeculative)
	{
		static CGameUIConVarRef cl_names_debug("cl_names_debug");
		if (cl_names_debug.GetInt())
			return "WWWWWWWWWWWWWWW";

//#if !defined( _X360 ) && !defined( NO_STEAM )
//		if (steamapicontext && steamapicontext->SteamUtils() &&
//			steamapicontext->SteamFriends() && steamapicontext->SteamUser())
//		{
//			int iIndex = m_mapUserXuidToName.Find(playerID);
//			if (iIndex == m_mapUserXuidToName.InvalidIndex())
//			{
//				char const* szName = steamapicontext->SteamFriends()->GetFriendPersonaName(playerID);
//				if (szName && *szName)
//				{
//					iIndex = m_mapUserXuidToName.Insert(playerID, szName);
//				}
//			}
//
//			if (iIndex != m_mapUserXuidToName.InvalidIndex())
//				return m_mapUserXuidToName.Element(iIndex).Get();
//		}
//#endif

		return szPlayerNameSpeculative;
	}
}