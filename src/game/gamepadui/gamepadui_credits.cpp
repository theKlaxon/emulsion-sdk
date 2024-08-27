#include "gamepadui_frame.h"
#include "gamepadui_interface.h"
#include "gamepadui_scroll.h"
#include "gamepadui_image.h"

#include "ienginevgui.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/IVGui.h"
#include "fmtstr.h"

#include "vgui_controls/ComboBox.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/ScrollBar.h"

#include "KeyValues.h"
#include "filesystem.h"

#include "tier0/memdbgon.h"

#define GAMEPADUI_CREDITS_SCHEME GAMEPADUI_RESOURCE_FOLDER "schemecredits.res"
#define GAMEPADUI_CREDITS_FILE GAMEPADUI_RESOURCE_FOLDER "credits.res"

struct GamepadUICredit {
public:
	GamepadUIString m_Name, m_Desc;

};

class GamepadUICreditsPanel : public GamepadUIFrame {
	DECLARE_CLASS_SIMPLE(GamepadUICreditsPanel, GamepadUIFrame)
public:
	GamepadUICreditsPanel(vgui::Panel* pParent, const char* pPanelName);
	~GamepadUICreditsPanel() {}

	void OnThink() override;
	void OnCommand(char const* pCommand) override;
	void ApplySchemeSettings(vgui::IScheme* pScheme) override;

	bool AtScrollLimit();

	void UpdateGradients();
	void LoadCredits();
	void LayoutCredits();
	void OnMouseWheeled(int nDelta);

private:

	CUtlVector<GamepadUIButton*> m_pCredits;
	GamepadUIScrollState m_ScrollState;

	bool m_bCanScroll;
	float m_flScrollClamp;
	float m_flScrollDelay;
	float m_flScrollDirAmt;
	float m_flScrollTiming;

	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsFade, "Credits.Fade", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsOffsetX, "Credits.OffsetX", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsOffsetY, "Credits.OffsetY", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsSpacing, "Credits.Spacing", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsStartScrollDelay, "Credits.StartScrollDelay", "0", SchemeValueTypes::Float);
	GAMEPADUI_PANEL_PROPERTY(float, m_flCreditsScrollRate, "Credits.ScrollRate", "0", SchemeValueTypes::Float);
};

GamepadUICreditsPanel::GamepadUICreditsPanel(vgui::Panel* pParent, const char* pPanelName) : BaseClass(pParent, pPanelName) {

	vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile(GAMEPADUI_CREDITS_SCHEME, "SchemePanel");
	SetScheme(hScheme);

	GetFrameTitle() = GamepadUIString("#GameUI_Credits_Title");
	SetFooterButtons(FooterButtons::Back);

	m_bCanScroll = false;
	m_flScrollClamp = 0.0f;
	m_flScrollDelay = GamepadUI::GetInstance().GetTime() + 2.0f;
	m_flScrollDirAmt = 0.0f;
	m_flScrollTiming = 0.0f;

	Activate();
	LoadCredits();
}

void GamepadUICreditsPanel::OnThink() {
	BaseClass::OnThink();

	float gptime = GamepadUI::GetInstance().GetTime();
	if (gptime >= m_flScrollDelay && m_bCanScroll) {

		if (AtScrollLimit())
			m_flScrollDirAmt *= -1;

		m_ScrollState.OnMouseWheeled(m_flScrollDirAmt * 160.0f, GamepadUI::GetInstance().GetTime());
		m_flScrollDelay = GamepadUI::GetInstance().GetTime() + m_flScrollTiming;
	}

	LayoutCredits();


}

void GamepadUICreditsPanel::OnCommand(char const* pCommand) {
	if (!V_strcmp(pCommand, "action_back"))
	{
		Close();
	}
	else
	{
		BaseClass::OnCommand(pCommand);
	}
}

void GamepadUICreditsPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	
	const char* pRate = pScheme->GetResourceString("Credits.ScrollRate");
	m_flScrollDirAmt = V_atof(pRate);

	const char* pTime = pScheme->GetResourceString("Credits.ScrollTiming");
	m_flScrollTiming = V_atof(pTime);

	m_bCanScroll = true;

	UpdateGradients();
}

bool GamepadUICreditsPanel::AtScrollLimit() {
	return (m_ScrollState.GetScrollProgress() >= m_flScrollClamp - 1.0f && m_ScrollState.GetScrollProgress() <= m_flScrollClamp) ||
			(m_ScrollState.GetScrollProgress() <= 1.0f);
}

void GamepadUICreditsPanel::UpdateGradients()
{
	const float flTime = GamepadUI::GetInstance().GetTime();
	GamepadUI::GetInstance().GetGradientHelper()->ResetTargets(flTime);
	GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient(GradientSide::Up, { 1.0f, 1.0f }, flTime);
	GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient(GradientSide::Down, { 1.0f, 0.5f }, flTime);
	GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient(GradientSide::Left, { 1.0f, 0.65f }, flTime);
	GamepadUI::GetInstance().GetGradientHelper()->SetTargetGradient(GradientSide::Right, { 0.65, 1.0f }, flTime);
}

void GamepadUICreditsPanel::LoadCredits() {

	KeyValues* pDataFile = new KeyValues("Credits");
	if (pDataFile) {

		if (pDataFile->LoadFromFile(g_pFullFileSystem, GAMEPADUI_CREDITS_FILE)) {

			for (KeyValues* pData = pDataFile->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey()) {

				const char* pTxt = pData->GetString("text", "Missing Credit");

				GamepadUIButton* pCredit = new GamepadUIButton(
					this, this,
					GAMEPADUI_CREDITS_SCHEME,
					pData->GetString("command"),
					pTxt,
					pData->GetString("description", "Missing Description"));
				
				pCredit->SetPriority(V_atoi(pData->GetString("priority", "0")));
				if (*pTxt == '\0')
					pCredit->SetVisible(false);
				else
					pCredit->SetVisible(true);

				m_pCredits.AddToTail(pCredit);
			}
		}
		pDataFile->deleteThis();
	}
}

void GamepadUICreditsPanel::LayoutCredits() {

	int nParentW, nParentH;
	GetParent()->GetSize(nParentW, nParentH);

	m_flScrollClamp = 0.0f;
	for (int i = 0; i < m_pCredits.Count(); i++)
	{
		int size = (m_pCredits[i]->GetTall() + m_flCreditsSpacing);

		if (i < m_pCredits.Count() - 2)
			m_flScrollClamp += size;
	}

	m_ScrollState.UpdateScrollBounds(0.0f, m_flScrollClamp);

	int previousSizes = 0;
	for (int i = 0; i < m_pCredits.Count(); i++)
	{
		int tall = m_pCredits[i]->GetTall();
		int size = (tall + m_flCreditsSpacing);

		int y = m_flCreditsOffsetY + previousSizes - m_ScrollState.GetScrollProgress();
		int fade = 255;
		if (y < m_flCreditsOffsetY)
			fade = (1.0f - clamp(-(y - m_flCreditsOffsetY) / m_flCreditsFade, 0.0f, 1.0f)) * 255.0f;
		if (y > nParentH - m_flCreditsFade)
			fade = (1.0f - clamp((y - (nParentH - m_flCreditsFade - size)) / m_flCreditsFade, 0.0f, 1.0f)) * 255.0f;
		if (m_pCredits[i]->HasFocus() && fade != 0)
			fade = 255;
		m_pCredits[i]->SetAlpha(fade);
		m_pCredits[i]->SetPos(m_flCreditsOffsetX, y);
		
		if (!m_pCredits[i]->GetButtonText().IsEmpty())
			m_pCredits[i]->SetVisible(true);
		previousSizes += size;
	}

	m_ScrollState.UpdateScrolling(2.0f, GamepadUI::GetInstance().GetTime());
	m_bCanScroll = true;
}

void GamepadUICreditsPanel::OnMouseWheeled(int nDelta)
{
	m_ScrollState.OnMouseWheeled(nDelta * 160.0f, GamepadUI::GetInstance().GetTime());
}

CON_COMMAND(gamepadui_opencredits, "")
{
	new GamepadUICreditsPanel(GamepadUI::GetInstance().GetBasePanel(), "CreditsPanel");
}
