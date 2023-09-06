#include "cbase.h"
#include "clientmode_shared.h"

// This is an implementation that does nearly nothing. It's only included to make Swarm Skeleton compile out of the box.
// You DEFINITELY want to replace it with your own class!
class ClientModeEmulsion : public ClientModeShared
{
public:
	DECLARE_CLASS(ClientModeEmulsion, ClientModeShared );

	~ClientModeEmulsion() {}

	void InitViewport();
	
	vgui::Panel					*GetViewport() { return m_pViewport; }
	vgui::AnimationController	*GetViewportAnimationController() { return m_pViewport->GetAnimationController(); }

	void OverrideAudioState( AudioState_t *pAudioState ) {}
	bool CanRecordDemo( char *errorMsg, int length ) const { return true; }
	void DoPostScreenSpaceEffects( const CViewSetup *pSetup ) {}

	void	SetBlurFade( float scale ) {}
	float	GetBlurFade( void ) { return 0; }
};

// There is always a fullscreen clientmode/viewport
class CEmulsionViewportFullscreen : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE(CEmulsionViewportFullscreen, CBaseViewport );

private:
	virtual void InitViewportSingletons( void )
	{
		SetAsFullscreenViewportInterface();
	}
};

class ClientModeEmulsionFullScreen : public ClientModeEmulsion
{
public:
	DECLARE_CLASS(ClientModeEmulsionFullScreen, ClientModeEmulsion);

	void InitViewport();
};