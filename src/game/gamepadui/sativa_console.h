#pragma once
#include "GameUI/IGameConsole.h"
#include "gamepadui_frame.h"

// Things the user typed in and hit submit/return with
//class CHistoryItem
//{
//public:
//	CHistoryItem(void);
//	CHistoryItem(const char* text, const char* extra = NULL);
//	CHistoryItem(const CHistoryItem& src);
//	~CHistoryItem(void);
//
//	const char* GetText() const;
//	const char* GetExtra() const;
//	void SetText(const char* text, const char* extra);
//	bool HasExtra() { return m_bHasExtra; }
//
//private:
//	char* m_text;
//	char* m_extraText;
//	bool		m_bHasExtra;
//};
//
//class SativaConsolePanel : public vgui::EditablePanel, public IConsoleDisplayFunc {
//public:
//
//protected:
//	friend class SativaConsoleDialogue;
//
//	vgui::RichText* m_pHistory;
//	vgui::TextEntry* m_pEntry;
//
//	GamepadUIButton* m_pSubmit;
//};
//
//class SativaConsoleDialogue : public GamepadUIFrame {
//public:
//
//	void Paint() override;
//	void OnKeyCodeTyped(vgui::KeyCode code) override;
//	void OnCommand(const char* pCommand) override;
//	void ApplySchemeSettings(vgui::IScheme* pScheme) override;
//	void UpdateGradients() override;
//
//private:
//
//	//GAMEPADUI_PANEL_PROPERTY(float, m_flGenericConfirmationOffsetX, "GenericConfirmation.OffsetX", "0", SchemeValueTypes::ProportionalFloat);
//	//GAMEPADUI_PANEL_PROPERTY(float, m_flGenericConfirmationOffsetY, "GenericConfirmation.OffsetY", "0", SchemeValueTypes::ProportionalFloat);
//};

class SativaConsole : public IGameConsole {
public:

	SativaConsole();

	// activates the console, makes it visible and brings it to the foreground
	virtual void Activate();

	virtual void Initialize();

	// hides the console
	virtual void Hide();

	// clears the console
	virtual void Clear();

	// return true if the console has focus
	virtual bool IsConsoleVisible();

	virtual void SetParent(int parent);

	// for use in the swarm code :/
	virtual void ActivateDelayed(float time);

	// we are the overrider here, stub it
	virtual void Override(IGameConsole* pConsole) {}

protected:

	bool m_bInitd;
	//SativaConsoleDialogue* m_pConsole;

};

