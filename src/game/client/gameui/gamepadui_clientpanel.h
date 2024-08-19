#pragma once
#include "vgui_controls/Panel.h"

namespace BaseModUI {

	class CGamepadUI_ClientPanel : public vgui::Panel {
		DECLARE_CLASS_SIMPLE(CGamepadUI_ClientPanel, vgui::Panel)
	public:
		CGamepadUI_ClientPanel() : BaseClass() {}
		~CGamepadUI_ClientPanel() {}

		void OnGameUIActivated() {
			COM_TimestampedLog("CGamepadUI_ClientPanel::OnGameUIActivated()");
			if (GetParent() == nullptr)
				return;
			
			GetParent()->SetVisible(true);
		}

		void OnGameUIHidden() {
			COM_TimestampedLog("CGamepadUI_ClientPanel::OnGameUIHidden()");

			if(GetParent() != nullptr)
				GetParent()->SetVisible(false);
		}
	};

}