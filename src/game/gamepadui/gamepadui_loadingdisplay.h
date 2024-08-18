#pragma once
#include "gamepadui_frame.h"
#include "gamepadui_image.h"

class GamepadUILoadingDisplay : public GamepadUIFrame {
	DECLARE_CLASS_SIMPLE(GamepadUILoadingDisplay, GamepadUIFrame);
public:
	GamepadUILoadingDisplay(vgui::Panel* pParent);

	void ApplySchemeSettings(vgui::IScheme* pScheme) override;

	void OnThink() override;
	void Paint() override;

private:
	
	void LayoutLoadingScreen();
	void PaintProgress();
	void PaintLogo();
	void PaintBackground();

	void GetLogoDrawPosition(int* x1, int* x2, int* y1, int* y2);
	void GetProgressDrawPosition(int* x1, int* x2, int* y1, int* y2);

	GamepadUIImage m_BackgroundImage;
	GamepadUIImage m_LogoImage;

	GAMEPADUI_PANEL_PROPERTY(float, m_flLogoOffsetX, "Logo.OffsetX", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flLogoOffsetY, "Logo.OffsetY", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flLogoWidth, "Logo.Width", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flLogoHeight, "Logo.Height", "0", SchemeValueTypes::ProportionalFloat);

	GAMEPADUI_PANEL_PROPERTY(float, m_flProgOffsetX, "Progress.OffsetX", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flProgOffsetY, "Progress.OffsetY", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flProgWidth, "Progress.Width", "0", SchemeValueTypes::ProportionalFloat);
	GAMEPADUI_PANEL_PROPERTY(float, m_flProgHeight, "Progress.Height", "0", SchemeValueTypes::ProportionalFloat);

};