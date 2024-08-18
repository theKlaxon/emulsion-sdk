#include "gamepadui_loadingdisplay.h"

#include "KeyValues.h"
#include "filesystem.h"

#define GAMEPADUI_LOADINGDISPLAY_FILE GAMEPADUI_RESOURCE_FOLDER "schemeloadingdisplay.res"

GamepadUILoadingDisplay::GamepadUILoadingDisplay(vgui::Panel* pParent)
    : BaseClass(pParent, "LoadingDisplay")
{
    vgui::HScheme hScheme = vgui::scheme()->LoadSchemeFromFile(GAMEPADUI_LOADINGDISPLAY_FILE, "SchemeLoadingDisplay");
    SetScheme(hScheme);
    SetPaintBackgroundEnabled(false);
}

void GamepadUILoadingDisplay::ApplySchemeSettings(vgui::IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);

    int nParentW, nParentH;
    GetParent()->GetSize(nParentW, nParentH);
    SetBounds(0, 0, nParentW, nParentH);

    const char* pImage = pScheme->GetResourceString("Background.Image");
    Msg(pImage);
    if (pImage && *pImage)
        m_BackgroundImage.SetImage(pImage);

    pImage = pScheme->GetResourceString("Logo.Image");
    if (pImage && *pImage)
        m_LogoImage.SetImage(pImage);

}

void GamepadUILoadingDisplay::OnThink()
{
    BaseClass::OnThink();

    LayoutLoadingScreen();
}

void GamepadUILoadingDisplay::Paint()
{
    BaseClass::Paint();

    PaintBackground();
    PaintLogo();
    PaintProgress();
}

void GamepadUILoadingDisplay::LayoutLoadingScreen()
{
    
}

void GamepadUILoadingDisplay::PaintProgress()
{

}

void GamepadUILoadingDisplay::PaintLogo()
{
    if (!m_LogoImage.IsValid())
        return;

    int x1, x2, y1, y2;
    GetLogoDrawPosition(&x1, &x2, &y1, &y2);

    vgui::surface()->DrawSetColor(Color(255, 255, 255, 255));
    vgui::surface()->DrawSetTexture(m_LogoImage);
    vgui::surface()->DrawTexturedRect(x1, y1, x2, y2);
    vgui::surface()->DrawSetTexture(0);
}

void GamepadUILoadingDisplay::PaintBackground()
{
    if (!m_BackgroundImage.IsValid())
        return;
}

void GamepadUILoadingDisplay::GetLogoDrawPosition(int* x1, int* x2, int* y1, int* y2)
{
    *x1 = 0, *x2 = 0, *y1 = 0, *y2 = 0;

    int nLogoY = GetTall() - (m_flLogoOffsetY);
    int nY1 = nLogoY;
    int nY2 = nY1 + m_flLogoHeight;
    int nX1 = m_flLogoOffsetX;
    int nX2 = nX1 + m_flLogoWidth;

    *x1 = nX1, *x2 = nX2, *y1 = nY1, *y2 = nY2;
}

void GamepadUILoadingDisplay::GetProgressDrawPosition(int* x1, int* x2, int* y1, int* y2)
{
    *x1 = 0, * x2 = 0, * y1 = 0, * y2 = 0;

    int nLogoY = GetTall() - (m_flProgOffsetY);
    int nY1 = nLogoY;
    int nY2 = nY1 + m_flProgHeight;
    int nX1 = m_flProgOffsetX;
    int nX2 = nX1 + m_flProgWidth;

    *x1 = nX1, * x2 = nX2, * y1 = nY1, * y2 = nY2;
}