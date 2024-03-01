#ifndef GAMEPADUI_BASEPANEL_H
#define GAMEPADUI_BASEPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "gamepadui_backgroundmovie.h"
#include "gamepadui_interface.h"

class GamepadUIMainMenu;

class GamepadUIBasePanel : public vgui::Panel
{
    DECLARE_CLASS_SIMPLE(GamepadUIBasePanel, vgui::Panel );
public:
    GamepadUIBasePanel( vgui::VPANEL parent );

    void ApplySchemeSettings( vgui::IScheme* pScheme ) override;

    GamepadUIMainMenu *GetMainMenuPanel() const;
    GamepadUIMovieBackground* GetMovieBackground() const;

    void OnMenuStateChanged();

    void ActivateBackgroundEffects();
    bool IsBackgroundMusicPlaying();
    bool StartBackgroundMusic( float flVolume );
    void ReleaseBackgroundMusic();

    void SetBackgroundMovie(const char* szMovieName);
    void SetBackgroundMovieEnabled(bool bEnabled);
    void PaintBackgroundMovie();

private:
    GamepadUIMainMenu *m_pMainMenu = NULL;
    GamepadUIMovieBackground* m_pMovie;

    int m_nBackgroundMusicGUID;
    bool m_bBackgroundMusicEnabled;
    bool m_bBackgroundMovieEnabled;
};

#endif // GAMEPADUI_BASEPANEL_H
