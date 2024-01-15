#include "cbase.h"
#include "UIGameData.h"

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

	void CUIGameData::OnGameUIPostInit()
	{
	}

	void CUIGameData::NeedConnectionProblemWaitScreen()
	{
	}

	void CUIGameData::ShowPasswordUI(char const*pchCurrentPW)
	{
	}
}