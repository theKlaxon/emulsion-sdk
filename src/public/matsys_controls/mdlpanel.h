//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef MDLPANEL_H
#define MDLPANEL_H

#ifdef _WIN32
#pragma once
#endif


#include "vgui_controls/Panel.h"
#include "datacache/imdlcache.h"
#include "materialsystem/materialsystemutil.h"
#include "matsys_controls/potterywheelpanel.h"
#include "tier3/mdlutils.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
namespace vgui
{
	class IScheme;
}


//-----------------------------------------------------------------------------
// MDL Viewer Panel
//-----------------------------------------------------------------------------
class CMDLPanel : public CPotteryWheelPanel
{
	DECLARE_CLASS_SIMPLE( CMDLPanel, CPotteryWheelPanel );

public:
	// constructor, destructor
	CMDLPanel( vgui::Panel *pParent, const char *pName );
	virtual ~CMDLPanel();

	// Overriden methods of vgui::Panel
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void OnTick();

	// Sets the current mdl
	virtual void SetMDL( MDLHandle_t handle );
	virtual void SetMDL( const char *pMDLName );
	void SetSkin(int nSkin);

	// Sets the camera to look at the model
	void LookAtMDL( );

	// Sets the current sequence
	void SetSequence( int nSequence );

	// Set the pose parameters
	void SetPoseParameters( const float *pPoseParameters, int nCount );

	// Set the overlay sequence layers
	void SetSequenceLayers( const MDLSquenceLayer_t *pSequenceLayers, int nCount );

	void SetCollsionModel( bool bVisible );
	void SetGroundGrid( bool bVisible );
	void SetWireFrame( bool bVisible );
	void SetLockView( bool bLocked );
	//void SetSkin( int nSkin );
	void SetLookAtCamera( bool bLookAtCamera );

	// Bounds.
	bool GetBoundingBox( Vector &vecBoundsMin, Vector &vecBoundsMax );
	bool GetBoundingSphere( Vector &vecCenter, float &flRadius );
	bool GetAttachment( const char *szAttachment, matrix3x4_t& matrixOut );
	bool GetAttachment( int iAttachmentNum, matrix3x4_t& matrixOut );

	void SetModelAnglesAndPosition( const QAngle &angRot, const Vector &vecPos );

	// Lock the camera position and angles to an attachment point
	void SetCameraAttachment(const char* pszAttachment);
	void SetRenderCaptureCameraAttachment(const char* pszAttachment);
	void SetDirectionalLightAttachment(int idx, const char* pszAttachment);

	void SetCameraOrientOverride(Vector vecNew) { m_vecCameraOrientOverride = vecNew; }
	void SetCameraOrientOverrideEnabled(bool bEnabled) { m_bCameraOrientOverrideEnabled = bEnabled; }
	bool IsCameraOrientOverrideEnabled(void) { return m_bCameraOrientOverrideEnabled; }

	void SetCameraPositionOverride(Vector vecNew) { m_vecCameraPositionOverride = vecNew; }
	void SetCameraPositionOverrideEnabled(bool bEnabled) { m_bCameraPositionOverrideEnabled = bEnabled; }
	bool IsCameraPositionOverrideEnabled(void) { return m_bCameraPositionOverrideEnabled; }

protected:
	void OnPostSetUpBonesPreDraw();
	void OnModelDrawPassStart(int iPass, CStudioHdr* pStudioHdr, int& nFlags);
	void OnModelDrawPassFinished(int iPass, CStudioHdr* pStudioHdr, int& nFlags);


	struct MDLData_t
	{
		CMDL		m_MDL;
		matrix3x4_t	m_MDLToWorld;
	};

	MDLData_t				m_RootMDL;
	CUtlVector<MDLData_t>	m_aMergeMDLs;

private:
	// paint it!
	void OnPaint3D();
	void OnMouseDoublePressed( vgui::MouseCode code );

	void DrawCollisionModel();
	void UpdateStudioRenderConfig( void );

	CTextureReference m_DefaultEnvCubemap;
	CTextureReference m_DefaultHDREnvCubemap;

	//bool	m_bDrawCollisionModel : 1;
	//bool	m_bGroundGrid : 1;
	//bool	m_bLockView : 1;
	//bool	m_bWireFrame : 1;
	//bool	m_bLookAtCamera : 1;

	float	m_PoseParameters[ MAXSTUDIOPOSEPARAM ];

	static const int MAX_SEQUENCE_LAYERS = 8;
	int					m_nNumSequenceLayers;
	MDLSquenceLayer_t	m_SequenceLayers[ MAX_SEQUENCE_LAYERS ];

	int		m_iCameraAttachment;
	int		m_iRenderCaptureCameraAttachment;
	int		m_iDirectionalLightAttachments[MATERIAL_MAX_LIGHT_COUNT];
	float	m_flAutoPlayTimeBase;

	bool	m_bDrawCollisionModel : 1;
	bool	m_bGroundGrid : 1;
	bool	m_bLockView : 1;
	bool	m_bWireFrame : 1;
	bool	m_bLookAtCamera : 1;
	bool	m_bAnimationPause : 1;

	bool	m_bCameraOrientOverrideEnabled;
	Vector	m_vecCameraOrientOverride;

	bool	m_bCameraPositionOverrideEnabled;
	Vector	m_vecCameraPositionOverride;
};


#endif // MDLPANEL_H
