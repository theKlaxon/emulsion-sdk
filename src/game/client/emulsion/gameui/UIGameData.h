#include "matchmaking/imatchframework.h"
#include "matchmaking/imatchsystem.h"
#include "matchmaking/iplayer.h"
#include "matchmaking/iplayermanager.h"
#include "matchmaking/iservermanager.h"

namespace BaseModUI
{
	class CUIGameData : public IMatchEventsSink
	{
		static CUIGameData* m_Singleton;
		static bool m_bModuleShutDown;

	public:
		static CUIGameData* Get();
		void ShutDown();

		void OnGameUIPostInit();
		void NeedConnectionProblemWaitScreen();
		void ShowPasswordUI(char const*pchCurrentPW);
	};
}