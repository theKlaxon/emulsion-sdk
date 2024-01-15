#include "cbase.h"
#include "clientmode_shared.h"

// This is an implementation that does nearly nothing. It's only included to make Swarm Skeleton compile out of the box.
// You DEFINITELY want to replace it with your own class!
class ClientModeSkeleton : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeSkeleton, ClientModeShared );

	~ClientModeSkeleton() {}

	void InitViewport();

	// Gets at the viewport, if there is one...
	vgui::Panel *GetViewport() { return m_pViewport; }

	// Gets at the viewports vgui panel animation controller, if there is one...
	vgui::AnimationController *GetViewportAnimationController() { return m_pViewport->GetAnimationController(); }

	void OverrideAudioState( AudioState_t *pAudioState ) {}
	
	bool	CanRecordDemo( char *errorMsg, int length ) const { return true; }

	void DoPostScreenSpaceEffects( const CViewSetup *pSetup ) {}

	void SetBlurFade( float scale ) {}
	float	GetBlurFade( void ) { return 0; }
};

// There is always a fullscreen clientmode/viewport
class CSkeletonViewportFullscreen : public CBaseViewport
{
private:
	DECLARE_CLASS_SIMPLE( CSkeletonViewportFullscreen, CBaseViewport );

private:
	virtual void InitViewportSingletons( void )
	{
		SetAsFullscreenViewportInterface();
	}
};

class ClientModeSkeletonFullScreen : public ClientModeSkeleton
{
public:
	DECLARE_CLASS( ClientModeSkeletonFullScreen, ClientModeSkeleton );

	void InitViewport();
};