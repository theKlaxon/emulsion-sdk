//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#pragma once

#ifdef _WIN32
#pragma once
#endif

#define OVERBRIGHT 2.0f
#define OO_OVERBRIGHT ( 1.0f / 2.0f )
#define GAMMA 2.2f
#define TEXGAMMA 2.2f

#include "tier1/interface.h"
#include "tier1/refcount.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "mathlib/vmatrix.h"
#include "appframework/IAppSystem.h"
#include "bitmap/imageformat.h"
#include "texture_group_names.h"
#include "vtf/vtf.h"
#include "materialsystem/deformations.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/IColorCorrection.h"
#include "refcount.h"

#if !defined( _X360 )
// NOTE: Disable this for l4d2 in general!!!  It allocates 4mb of rendertargets and causes Release/Reallocation of rendertargets.
//#define FEATURE_SUBD_SUPPORT
#endif

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
struct MaterialSystem_Config_t;
class VMatrix;
struct matrix3x4_t;
class ITexture;
struct MaterialSystemHardwareIdentifier_t;
class KeyValues;
class IShader;
class IVertexTexture;
class IMorph;
class IMatRenderContext;
class ICallQueue;
struct MorphWeight_t;
class IFileList;
struct VertexStreamSpec_t;
struct ShaderStencilState_t;
struct MeshInstanceData_t;
class IClientMaterialSystem;
class CPaintMaterial;
class IPaintMapDataManager;
class IPaintMapTextureManager;
class IMaterialSystem;

//-----------------------------------------------------------------------------
// The vertex format type
//-----------------------------------------------------------------------------
typedef uint64 VertexFormat_t;

//-----------------------------------------------------------------------------
// important enumeration
//-----------------------------------------------------------------------------
enum ShaderParamType_t 
{ 
	SHADER_PARAM_TYPE_TEXTURE, 
	SHADER_PARAM_TYPE_INTEGER,
	SHADER_PARAM_TYPE_COLOR,
	SHADER_PARAM_TYPE_VEC2,
	SHADER_PARAM_TYPE_VEC3,
	SHADER_PARAM_TYPE_VEC4,
	SHADER_PARAM_TYPE_ENVMAP,	// obsolete
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_FOURCC,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_MATERIAL,
	SHADER_PARAM_TYPE_STRING,
};

enum MaterialMatrixMode_t
{
	MATERIAL_VIEW = 0,
	MATERIAL_PROJECTION,

	MATERIAL_MATRIX_UNUSED0,
	MATERIAL_MATRIX_UNUSED1,
	MATERIAL_MATRIX_UNUSED2,
	MATERIAL_MATRIX_UNUSED3,
	MATERIAL_MATRIX_UNUSED4,
	MATERIAL_MATRIX_UNUSED5,
	MATERIAL_MATRIX_UNUSED6,
	MATERIAL_MATRIX_UNUSED7,

	MATERIAL_MODEL,

	// Total number of matrices
	NUM_MATRIX_MODES = MATERIAL_MODEL+1,
};

// FIXME: How do I specify the actual number of matrix modes?
const int NUM_MODEL_TRANSFORMS = 53;
const int MATERIAL_MODEL_MAX = MATERIAL_MODEL + NUM_MODEL_TRANSFORMS;

enum MaterialPrimitiveType_t 
{ 
	MATERIAL_POINTS			= 0x0,
	MATERIAL_LINES,
	MATERIAL_TRIANGLES,
	MATERIAL_TRIANGLE_STRIP,
	MATERIAL_LINE_STRIP,
	MATERIAL_LINE_LOOP,	// a single line loop
	MATERIAL_POLYGON,	// this is a *single* polygon
	MATERIAL_QUADS,
	MATERIAL_SUBD_QUADS_EXTRA, // Extraordinary sub-d quads
	MATERIAL_SUBD_QUADS_REG,   // Regular sub-d quads
	MATERIAL_INSTANCED_QUADS, // (X360) like MATERIAL_QUADS, but uses vertex instancing

	// This is used for static meshes that contain multiple types of
	// primitive types.	When calling draw, you'll need to specify
	// a primitive type.
	MATERIAL_HETEROGENOUS
};

enum TessellationMode_t
{
	TESSELLATION_MODE_DISABLED = 0,
	TESSELLATION_MODE_ACC_PATCHES_EXTRA,
	TESSELLATION_MODE_ACC_PATCHES_REG
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};

// acceptable property values for MATERIAL_PROPERTY_OPACITY
enum MaterialPropertyOpacityTypes_t
{
	MATERIAL_ALPHATEST = 0,
	MATERIAL_OPAQUE,
	MATERIAL_TRANSLUCENT
};

enum MaterialBufferTypes_t
{
	MATERIAL_FRONT = 0,
	MATERIAL_BACK
};

enum MaterialCullMode_t
{
	MATERIAL_CULLMODE_CCW,	// this culls polygons with counterclockwise winding
	MATERIAL_CULLMODE_CW	// this culls polygons with clockwise winding
};

enum MaterialIndexFormat_t
{
	MATERIAL_INDEX_FORMAT_UNKNOWN = -1,
	MATERIAL_INDEX_FORMAT_16BIT = 0,
	MATERIAL_INDEX_FORMAT_32BIT,
};

enum MaterialFogMode_t
{
	MATERIAL_FOG_NONE,
	MATERIAL_FOG_LINEAR,
	MATERIAL_FOG_LINEAR_BELOW_FOG_Z,
};

enum MaterialHeightClipMode_t
{
	MATERIAL_HEIGHTCLIPMODE_DISABLE,
	MATERIAL_HEIGHTCLIPMODE_RENDER_ABOVE_HEIGHT,
	MATERIAL_HEIGHTCLIPMODE_RENDER_BELOW_HEIGHT
};

enum MaterialNonInteractiveMode_t
{
	MATERIAL_NON_INTERACTIVE_MODE_NONE = -1,
	MATERIAL_NON_INTERACTIVE_MODE_STARTUP = 0,
	MATERIAL_NON_INTERACTIVE_MODE_LEVEL_LOAD,

	MATERIAL_NON_INTERACTIVE_MODE_COUNT,
};


//-----------------------------------------------------------------------------
// Special morph used in decalling pass
//-----------------------------------------------------------------------------
#define MATERIAL_MORPH_DECAL ( (IMorph*)1 )


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

enum MaterialContextType_t
{
	MATERIAL_HARDWARE_CONTEXT,
	MATERIAL_QUEUED_CONTEXT,
	MATERIAL_NULL_CONTEXT
};


//-----------------------------------------------------------------------------
// Light structure
//-----------------------------------------------------------------------------
#include "mathlib/lightdesc.h"

enum
{
	MATERIAL_MAX_LIGHT_COUNT = 4,
};

struct MaterialLightingState_t
{
	Vector			m_vecAmbientCube[6];		// ambient, and lights that aren't in locallight[]
	Vector			m_vecLightingOrigin;		// The position from which lighting state was computed
	int				m_nLocalLightCount;
	LightDesc_t		m_pLocalLightDesc[MATERIAL_MAX_LIGHT_COUNT];

	MaterialLightingState_t &operator=( const MaterialLightingState_t &src )
	{
		memcpy( this, &src, sizeof(MaterialLightingState_t) - MATERIAL_MAX_LIGHT_COUNT * sizeof(LightDesc_t) );
		memcpy( m_pLocalLightDesc, &src.m_pLocalLightDesc, src.m_nLocalLightCount * sizeof(LightDesc_t) );
		return *this;
	}
};



#define CREATERENDERTARGETFLAGS_HDR				0x00000001
#define CREATERENDERTARGETFLAGS_AUTOMIPMAP		0x00000002
#define CREATERENDERTARGETFLAGS_UNFILTERABLE_OK 0x00000004
// XBOX ONLY:
#define CREATERENDERTARGETFLAGS_NOEDRAM			0x00000008 // inhibit allocation in 360 EDRAM
#define CREATERENDERTARGETFLAGS_TEMP			0x00000010 // only allocates memory upon first resolve, destroyed at level end


//-----------------------------------------------------------------------------
// Enumeration for the various fields capable of being morphed
//-----------------------------------------------------------------------------
enum MorphFormatFlags_t
{
	MORPH_POSITION	= 0x0001,	// 3D
	MORPH_NORMAL	= 0x0002,	// 3D
	MORPH_WRINKLE	= 0x0004,	// 1D
	MORPH_SPEED		= 0x0008,	// 1D
	MORPH_SIDE		= 0x0010,	// 1D
};


//-----------------------------------------------------------------------------
// The morph format type
//-----------------------------------------------------------------------------
typedef unsigned int MorphFormat_t;


//-----------------------------------------------------------------------------
// Standard lightmaps
//-----------------------------------------------------------------------------
enum StandardLightmap_t
{
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


struct MaterialSystem_SortInfo_t
{
	IMaterial	*material;
	int			lightmapPageID;
};


#define MAX_FB_TEXTURES 4

//-----------------------------------------------------------------------------
// Information about each adapter
//-----------------------------------------------------------------------------
enum
{
	MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialAdapterInfo_t
{
	char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;			// This is the *preferred* dx support level
	int m_nMinDXSupportLevel;
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
};


//-----------------------------------------------------------------------------
// Video mode info..
//-----------------------------------------------------------------------------
struct MaterialVideoMode_t
{
	int m_Width;			// if width and height are 0 and you select 
	int m_Height;			// windowed mode, it'll use the window size
	ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
	int m_RefreshRate;		// 0 == default (ignored for windowed mode)
};


//--------------------------------------------------------------------------------
// Uberlight parameters
//--------------------------------------------------------------------------------
struct UberlightState_t
{
	UberlightState_t()
	{
		m_fNearEdge 	= 2.0f;
		m_fFarEdge  	= 100.0f;
		m_fCutOn    	= 10.0f;
		m_fCutOff   	= 650.0f;
		m_fShearx   	= 0.0f;
		m_fSheary   	= 0.0f;
		m_fWidth    	= 0.3f;
		m_fWedge    	= 0.05f;
		m_fHeight		= 0.3f;
		m_fHedge		= 0.05f;
		m_fRoundness	= 0.8f;
	}

	float m_fNearEdge;
	float m_fFarEdge;
	float m_fCutOn;
	float m_fCutOff;
	float m_fShearx;
	float m_fSheary;
	float m_fWidth;
	float m_fWedge;
	float m_fHeight;
	float m_fHedge;
	float m_fRoundness;

	IMPLEMENT_OPERATOR_EQUAL( UberlightState_t );
};

// fixme: should move this into something else.
struct FlashlightState_t
{
	FlashlightState_t()
	{
		m_bEnableShadows = false;						// Provide reasonable defaults for shadow depth mapping parameters
		m_bDrawShadowFrustum = false;
		m_flShadowMapResolution = 1024.0f;
		//m_flShadowFilterSize = 3.0f;
		m_flShadowFilterSize = 0.2f;
		m_flShadowSlopeScaleDepthBias = 16.0f;
		m_flShadowDepthBias = 0.0005f;
		m_flShadowJitterSeed = 0.0f;
		m_flShadowAtten = 0.0f;
		m_flAmbientOcclusion = 0.0f;
		m_nShadowQuality = 0;
		m_bShadowHighRes = false;

		m_bScissor = false; 
		m_nLeft = -1;
		m_nTop = -1;
		m_nRight = -1;
		m_nBottom = -1;

		m_bUberlight = false;

		m_bVolumetric = false;
		m_flNoiseStrength = 0.8f;
		m_flFlashlightTime = 0.0f;
		m_nNumPlanes = 64;
		m_flPlaneOffset = 0.0f;
		m_flVolumetricIntensity = 1.0f;

		m_bOrtho = false;
		m_fOrthoLeft = -1.0f;
		m_fOrthoRight = 1.0f;
		m_fOrthoTop = -1.0f;
		m_fOrthoBottom = 1.0f;

		m_fBrightnessScale = 1.0f;
		m_pSpotlightTexture = NULL;
		m_pProjectedMaterial = NULL;
		m_bGlobalLight = false;

		//m_bSimpleProjection = false;
		m_flProjectionSize = 500.0f;
		m_flProjectionRotation = 0.0f;
	}

	Vector m_vecLightOrigin;
	Quaternion m_quatOrientation;
	float m_NearZ;
	float m_FarZ;
	float m_fHorizontalFOVDegrees;
	float m_fVerticalFOVDegrees;

	bool  m_bOrtho;
	float m_fOrthoLeft;
	float m_fOrthoRight;
	float m_fOrthoTop;
	float m_fOrthoBottom;

	float m_fQuadraticAtten;
	float m_fLinearAtten;
	float m_fConstantAtten;
	float m_FarZAtten;
	float m_Color[4];
	float m_fBrightnessScale;
	ITexture *m_pSpotlightTexture;
	IMaterial *m_pProjectedMaterial;
	int m_nSpotlightTextureFrame;
	//bool m_bGlobalLight;

	// Shadow depth mapping parameters
	bool  m_bEnableShadows;
	bool  m_bDrawShadowFrustum;
	float m_flShadowMapResolution;
	float m_flShadowFilterSize;
	float m_flShadowSlopeScaleDepthBias;
	float m_flShadowDepthBias;
	float m_flShadowJitterSeed;
	float m_flShadowAtten;
	float m_flAmbientOcclusion;
	int   m_nShadowQuality;
	bool  m_bShadowHighRes;

	// simple projection
	//bool	m_bSimpleProjection;
	float	m_flProjectionSize;
	float	m_flProjectionRotation;

	// Uberlight parameters
	bool m_bUberlight;
	UberlightState_t m_uberlightState;

	bool m_bVolumetric;
	float m_flNoiseStrength;
	float m_flFlashlightTime;
	int m_nNumPlanes;
	float m_flPlaneOffset;
	float m_flVolumetricIntensity;

	bool m_bGlobalLight; // m_bShareBetweenSplitScreenPlayers in p2

	// Getters for scissor members
	bool DoScissor() const { return m_bScissor; }
	int GetLeft()	 const { return m_nLeft; }
	int GetTop()	 const { return m_nTop; }
	int GetRight()	 const { return m_nRight; }
	int GetBottom()	 const { return m_nBottom; }

private:

	friend class CShadowMgr;

	bool m_bScissor; 
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBottom;

	IMPLEMENT_OPERATOR_EQUAL( FlashlightState_t );
};

//-----------------------------------------------------------------------------
// Flags to be used with the Init call
//-----------------------------------------------------------------------------
enum MaterialInitFlags_t
{
	MATERIAL_INIT_ALLOCATE_FULLSCREEN_TEXTURE = 0x2,
	MATERIAL_INIT_REFERENCE_RASTERIZER	= 0x4,
};

//-----------------------------------------------------------------------------
// Flags to specify type of depth buffer used with RT
//-----------------------------------------------------------------------------

// GR - this is to add RT with no depth buffer bound

enum MaterialRenderTargetDepth_t
{
	MATERIAL_RT_DEPTH_SHARED   = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE     = 0x2,
	MATERIAL_RT_DEPTH_ONLY	   = 0x3,
};

//-----------------------------------------------------------------------------
// A function to be called when we need to release all vertex buffers
// NOTE: The restore function will tell the caller if all the vertex formats
// changed so that it can flush caches, etc. if it needs to (for dxlevel support)
//-----------------------------------------------------------------------------
enum RestoreChangeFlags_t
{
	MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
	MATERIAL_RESTORE_RELEASE_MANAGED_RESOURCES = 0x2,
};


// NOTE: All size modes will force the render target to be smaller than or equal to
// the size of the framebuffer.
enum RenderTargetSizeMode_t
{
	RT_SIZE_NO_CHANGE=0,			// Only allowed for render targets that don't want a depth buffer
	// (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
	RT_SIZE_DEFAULT=1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
	RT_SIZE_PICMIP=2,				// Apply picmip to the render target's width and height.
	RT_SIZE_HDR=3,					// frame_buffer_width / 4
	//RT_SIZE_FULL_FRAME_BUFFER=4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
	RT_SIZE_FULL_FRAME_BUFFER=6,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
	//RT_SIZE_OFFSCREEN=5,			// Target of specified size, don't mess with dimensions
	RT_SIZE_OFFSCREEN=10,			// Target of specified size, don't mess with dimensions
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP=6 // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
};

typedef void (*MaterialBufferReleaseFunc_t)( int nChangeFlags );	// see RestoreChangeFlags_t
typedef void (*MaterialBufferRestoreFunc_t)( int nChangeFlags );	// see RestoreChangeFlags_t
typedef void (*ModeChangeCallbackFunc_t)( void );
typedef void (*EndFrameCleanupFunc_t)( void );

//typedef int VertexBufferHandle_t;
typedef unsigned short MaterialHandle_t;

DECLARE_POINTER_HANDLE( OcclusionQueryObjectHandle_t );
#define INVALID_OCCLUSION_QUERY_OBJECT_HANDLE ( (OcclusionQueryObjectHandle_t)0 )

class IMaterialProxyFactory;
class ITexture;
class IMaterialSystemHardwareConfig;
class CShadowMgr;

DECLARE_POINTER_HANDLE( MaterialLock_t );

//-----------------------------------------------------------------------------
// Information about a material texture
//-----------------------------------------------------------------------------

struct MaterialTextureInfo_t
{
	// Exclude information:
	//		-1	texture is not subject to exclude-handling
	//		 0	texture is completely excluded
	//		>0	texture is clamped according to exclude-instruction
	int iExcludeInformation;
};

struct AspectRatioInfo_t
{
	bool m_bIsWidescreen;
	bool m_bIsHidef;
	float m_flFrameBufferAspectRatio; // width / height of framebuffer in pixels
	float m_flPhysicalAspectRatio; // width / height of the physical display in real-life units
	float m_flFrameBuffertoPhysicalScalar; // m_flPhysicalAspectRatio / m_flFrameBufferAspectRatio
	float m_flPhysicalToFrameBufferScalar; // m_flFrameBufferAspectRatio / m_flPhysicalAspectRatio
	bool m_bInitialized;

	AspectRatioInfo_t() :
		m_bIsWidescreen(false),
		m_bIsHidef(false),
		m_flFrameBufferAspectRatio(4.0f / 3.0f),
		m_flPhysicalAspectRatio(4.0f / 3.0f),
		m_flFrameBuffertoPhysicalScalar(1.0f),
		m_flPhysicalToFrameBufferScalar(1.0f),
		m_bInitialized(false)
	{
	}
};

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

abstract_class IMaterialSystemNew : public IAppSystem
{
public:

	// Placeholder for API revision
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	//---------------------------------------------------------
	// Initialization and shutdown
	//---------------------------------------------------------

	// Call this to initialize the material system
	// returns a method to create interfaces in the shader dll
	virtual CreateInterfaceFn	Init( char const* pShaderAPIDLL, 
		IMaterialProxyFactory *pMaterialProxyFactory,
		CreateInterfaceFn fileSystemFactory,
		CreateInterfaceFn cvarFactory=NULL ) = 0;

	// Call this to set an explicit shader version to use 
	// Must be called before Init().
	virtual void				SetShaderAPI( char const *pShaderAPIDLL ) = 0;

	// Must be called before Init(), if you're going to call it at all...
	virtual void				SetAdapter( int nAdapter, int nFlags ) = 0;

	// Call this when the mod has been set up, which may occur after init
	// At this point, the game + gamebin paths have been set up
	virtual void				ModInit() = 0;
	virtual void				ModShutdown() = 0;

	//---------------------------------------------------------
	//
	//---------------------------------------------------------
	virtual void				SetThreadMode( MaterialThreadMode_t mode, int nServiceThread = -1 ) = 0;
	virtual MaterialThreadMode_t GetThreadMode() = 0;
	virtual bool				IsRenderThreadSafe() = 0;
	virtual void				ExecuteQueued() = 0;

	virtual void OnDebugEvent(const char* pEvent = "") {}

	//---------------------------------------------------------
	// Config management
	//---------------------------------------------------------
	
	virtual IMaterialSystemHardwareConfig *GetHardwareConfig( const char *pVersion, int *returnCode ) = 0;

	// Call this before rendering each frame with the current config
	// for the material system.
	// Will do whatever is necessary to get the material system into the correct state
	// upon configuration change. .doesn't much else otherwise.
	virtual bool				UpdateConfig( bool bForceUpdate ) = 0;

	// Force this to be the config; update all material system convars to match the state
	// return true if lightmaps need to be redownloaded
	virtual bool				OverrideConfig( const MaterialSystem_Config_t &config, bool bForceUpdate ) = 0;

	// Get the current config for this video card (as last set by UpdateConfig)
	virtual const MaterialSystem_Config_t &GetCurrentConfigForVideoCard() const = 0;

	// Gets *recommended* configuration information associated with the display card, 
	// given a particular dx level to run under. 
	// Use dxlevel 0 to use the recommended dx level.
	// The function returns false if an invalid dxlevel was specified

	// UNDONE: To find out all convars affected by configuration, we'll need to change
	// the dxsupport.pl program to output all column headers into a single keyvalue block
	// and then we would read that in, and send it back to the client
	virtual bool				GetRecommendedConfigurationInfo( int nDXLevel, KeyValues * pKeyValues ) = 0;


	// -----------------------------------------------------------
	// Device methods
	// -----------------------------------------------------------

	// Gets the number of adapters...
	virtual int					GetDisplayAdapterCount() const = 0;

	// Returns the current adapter in use
	virtual int					GetCurrentAdapter() const = 0;

	// Returns info about each adapter
	virtual void				GetDisplayAdapterInfo( int adapter, MaterialAdapterInfo_t& info ) const = 0;

	// Returns the number of modes
	virtual int					GetModeCount( int adapter ) const = 0;

	// Returns mode information..
	virtual void				GetModeInfo( int adapter, int mode, MaterialVideoMode_t& info ) const = 0;

	virtual void				AddModeChangeCallBack( ModeChangeCallbackFunc_t func ) = 0;

	// Returns the mode info for the current display device
	virtual void				GetDisplayMode( MaterialVideoMode_t& mode ) const = 0;

	// Sets the mode...
	virtual bool				SetMode( void* hwnd, const MaterialSystem_Config_t &config ) = 0;

	virtual bool				SupportsMSAAMode( int nMSAAMode ) = 0;

	// FIXME: REMOVE! Get video card identitier
	virtual const MaterialSystemHardwareIdentifier_t &GetVideoCardIdentifier( void ) const = 0;

	// Use this to spew information about the 3D layer 
	virtual void				SpewDriverInfo() const = 0;

	// Get the image format of the back buffer. . useful when creating render targets, etc.
	virtual void				GetBackBufferDimensions( int &width, int &height) const = 0;
	virtual ImageFormat			GetBackBufferFormat() const = 0;

	virtual const AspectRatioInfo_t& GetAspectRatioInfo() = 0;

	virtual bool				SupportsHDRMode( HDRType_t nHDRModede ) = 0;


	// -----------------------------------------------------------
	// Window methods
	// -----------------------------------------------------------

	// Creates/ destroys a child window
	virtual bool				AddView( void* hwnd ) = 0;
	virtual void				RemoveView( void* hwnd ) = 0;

	// Sets the view
	virtual void				SetView( void* hwnd ) = 0;
	
	// -----------------------------------------------------------
	// Control flow
	// -----------------------------------------------------------

	virtual void				BeginFrame( float frameTime ) = 0;
	virtual void				EndFrame( ) = 0;
	virtual void				Flush( bool flushHardware = false ) = 0;

	virtual unsigned int		GetCurrentFrameCount() = 0; // 0x00017dc0

	/// FIXME: This stuff needs to be cleaned up and abstracted.
	// Stuff that gets exported to the launcher through the engine
	virtual void				SwapBuffers( ) = 0;

	// Flushes managed textures from the texture cacher
	virtual void				EvictManagedResources() = 0;

	virtual void				ReleaseResources(void) = 0;
	virtual void				ReacquireResources(void ) = 0;
	
	// -----------------------------------------------------------
	// Device loss/restore
	// -----------------------------------------------------------

	// Installs a function to be called when we need to release vertex buffers + textures
	virtual void				AddReleaseFunc( MaterialBufferReleaseFunc_t func ) = 0;
	virtual void				RemoveReleaseFunc( MaterialBufferReleaseFunc_t func ) = 0;

	// Installs a function to be called when we need to restore vertex buffers
	virtual void				AddRestoreFunc( MaterialBufferRestoreFunc_t func ) = 0;
	virtual void				RemoveRestoreFunc( MaterialBufferRestoreFunc_t func ) = 0;

	// Installs a function to be called when we need to delete objects at the end of the render frame
	virtual void				AddEndFrameCleanupFunc( EndFrameCleanupFunc_t func ) = 0;
	virtual void				RemoveEndFrameCleanupFunc( EndFrameCleanupFunc_t func ) = 0;

	virtual void				OnLevelShutdown() = 0;
	virtual bool				AddOnLevelShutdownFunc(void* par1, void* par2) = 0;
	virtual bool				RemoveOnLevelShutdownFunc(void* par1, void* par2) = 0;
	
	// Release temporary HW memory...
	virtual void				ResetTempHWMemory( bool bExitingLevel = false ) = 0;

	// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
	virtual void				HandleDeviceLost() = 0;
	
	// -----------------------------------------------------------
	// Shaders
	// -----------------------------------------------------------

	// Used to iterate over all shaders for editing purposes
	// GetShaders returns the number of shaders it actually found
	virtual int					ShaderCount() const = 0;
	virtual int					GetShaders( int nFirstShader, int nMaxCount, IShader **ppShaderList ) const = 0;

	// FIXME: Is there a better way of doing this?
	// Returns shader flag names for editors to be able to edit them
	virtual int					ShaderFlagCount() const = 0;
	virtual const char *		ShaderFlagName( int nIndex ) const = 0;

	// Gets the actual shader fallback for a particular shader
	virtual void				GetShaderFallback( const char *pShaderName, char *pFallbackShader, int nFallbackLength ) = 0;

	// -----------------------------------------------------------
	// Material proxies
	// -----------------------------------------------------------

	virtual IMaterialProxyFactory *GetMaterialProxyFactory() = 0;

	// Sets the material proxy factory. Calling this causes all materials to be uncached.
	virtual void				SetMaterialProxyFactory( IMaterialProxyFactory* pFactory ) = 0;

	// -----------------------------------------------------------
	// Editor mode
	// -----------------------------------------------------------

	// Used to enable editor materials. Must be called before Init.
	virtual void				EnableEditorMaterials() = 0;
	virtual void                EnableGBuffers() = 0;

	// -----------------------------------------------------------
	// Stub mode mode
	// -----------------------------------------------------------

	// Force it to ignore Draw calls.
	virtual void				SetInStubMode( bool bInStubMode ) = 0;

	//---------------------------------------------------------
	// Debug support
	//---------------------------------------------------------

	virtual void				DebugPrintUsedMaterials( const char *pSearchSubString, bool bVerbose ) = 0;
	virtual void				DebugPrintUsedTextures( void ) = 0;

	virtual void				ToggleSuppressMaterial( char const* pMaterialName ) = 0;
	virtual void				ToggleDebugMaterial( char const* pMaterialName ) = 0;

	//---------------------------------------------------------
	// Misc features
	//---------------------------------------------------------
	//returns whether fast clipping is being used or not - needed to be exposed for better per-object clip behavior
	virtual bool				UsingFastClipping( void ) = 0;

	virtual int					StencilBufferBits( void ) = 0; //number of bits per pixel in the stencil buffer

	//---------------------------------------------------------
	// Material and texture management
	//---------------------------------------------------------

	// uncache all materials. .  good for forcing reload of materials.
	virtual void				UncacheAllMaterials( ) = 0;

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void				UncacheUnusedMaterials( bool bRecomputeStateSnapshots = false ) = 0;

	// Load any materials into memory that are to be used as determined
	// by the IMaterial's reference count.
	virtual void				CacheUsedMaterials( ) = 0;

	// Force all textures to be reloaded from disk.
	virtual void				ReloadTextures( ) = 0;

	// Reloads materials
	virtual void				ReloadMaterials( const char *pSubString = NULL ) = 0;

	// Create a procedural material. The keyvalues looks like a VMT file
	virtual IMaterial *			CreateMaterial( const char *pMaterialName, KeyValues *pVMTKeyValues ) = 0;

	// Find a material by name.
	// The name of a material is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	// eg. "dev/dev_bumptest" refers to somethign similar to:
	// "d:/hl2/hl2/materials/dev/dev_bumptest.vmt"
	//
	// Most of the texture groups for pTextureGroupName are listed in texture_group_names.h.
	// 
	// Note: if the material can't be found, this returns a checkerboard material. You can 
	// find out if you have that material by calling IMaterial::IsErrorMaterial().
	// (Or use the global IsErrorMaterial function, which checks if it's null too).
	virtual IMaterial *			FindMaterial( char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL ) = 0;

	virtual bool IsMaterialLoaded(const char* par1) = 0;

	//---------------------------------
	// This is the interface for knowing what materials are available
	// is to use the following functions to get a list of materials.  The
	// material names will have the full path to the material, and that is the 
	// only way that the directory structure of the materials will be seen through this
	// interface.
	// NOTE:  This is mostly for worldcraft to get a list of materials to put
	// in the "texture" browser.in Worldcraft
	virtual MaterialHandle_t	FirstMaterial() const = 0;

	// returns InvalidMaterial if there isn't another material.
	// WARNING: you must call GetNextMaterial until it returns NULL, 
	// otherwise there will be a memory leak.
	virtual MaterialHandle_t	NextMaterial( MaterialHandle_t h ) const = 0;

	// This is the invalid material
	virtual MaterialHandle_t	InvalidMaterial() const = 0;

	// Returns a particular material
	virtual IMaterial*			GetMaterial( MaterialHandle_t h ) const = 0;

	// Get the total number of materials in the system.  These aren't just the used
	// materials, but the complete collection.
	virtual int					GetNumMaterials( ) const = 0;

	//---------------------------------

	virtual ITexture *			FindTexture( char const* pTextureName, const char *pTextureGroupName, bool complain = true, int flags = 0 ) = 0;

	// Checks to see if a particular texture is loaded
	virtual bool				IsTextureLoaded( char const* pTextureName ) const = 0;

	// Creates a procedural texture
	virtual ITexture *			CreateProceduralTexture( const char	*pTextureName, 
		const char *pTextureGroupName, 
		int w, 
		int h, 
		ImageFormat fmt, 
		int nFlags ) = 0;

	//
	// Render targets
	//
	virtual void				BeginRenderTargetAllocation() = 0;
	virtual void				EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources

	// Creates a render target
	// If depth == true, a depth buffer is also allocated. If not, then
	// the screen's depth buffer is used.
	// Creates a texture for use as a render target
	virtual ITexture* CreateRenderTargetTexture(int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat	format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;

	virtual ITexture* CreateNamedRenderTargetTextureEx(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		unsigned int renderTargetFlags = 0) = 0;

	virtual ITexture* CreateNamedRenderTargetTexture(const char* pRTName,
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		bool bClampTexCoords = true,
		bool bAutoMipMap = false) = 0;

	// Must be called between the above Begin-End calls!
	virtual ITexture* CreateNamedRenderTargetTextureEx2(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		unsigned int renderTargetFlags = 0) = 0;

	// -----------------------------------------------------------
	// Lightmaps
	// -----------------------------------------------------------

	// To allocate lightmaps, sort the whole world by material twice.
	// The first time through, call AllocateLightmap for every surface.
	// that has a lightmap.
	// The second time through, call AllocateWhiteLightmap for every 
	// surface that expects to use shaders that expect lightmaps.
	virtual void				BeginLightmapAllocation( ) = 0;
	virtual void				EndLightmapAllocation( ) = 0;
	virtual void				CleanupLightmaps() = 0;

	// returns the sorting id for this surface
	virtual int 				AllocateLightmap( int width, int height, 
		int offsetIntoLightmapPage[2],
		IMaterial *pMaterial ) = 0;
	// returns the sorting id for this surface
	virtual int					AllocateWhiteLightmap( IMaterial *pMaterial ) = 0;

	// lightmaps are in linear color space
	// lightmapPageID is returned by GetLightmapPageIDForSortID
	// lightmapSize and offsetIntoLightmapPage are returned by AllocateLightmap.
	// You should never call UpdateLightmap for a lightmap allocated through
	// AllocateWhiteLightmap.
	virtual void				UpdateLightmap( int lightmapPageID, int lightmapSize[2],
		int offsetIntoLightmapPage[2], 
		float *pFloatImage, float *pFloatImageBump1,
		float *pFloatImageBump2, float *pFloatImageBump3 ) = 0;

	// fixme: could just be an array of ints for lightmapPageIDs since the material
	// for a surface is already known.
	virtual int					GetNumSortIDs( ) = 0;
	virtual void				GetSortInfo( MaterialSystem_SortInfo_t *sortInfoArray ) = 0;

	// Read the page size of an existing lightmap by sort id (returned from AllocateLightmap())
	virtual void				GetLightmapPageSize( int lightmap, int *width, int *height ) const = 0;

	virtual void				ResetMaterialLightmapPageInfo() = 0;

	virtual bool				IsStereoSupported() = 0;
	virtual bool				IsStereoActiveThisFrame() const = 0;

	virtual void				NVStereoUpdate() = 0;
	virtual void				ClearBuffers( bool bClearColor, bool bClearDepth, bool bClearStencil = false ) = 0;

	// -----------------------------------------------------------
	// X360 specifics
	// -----------------------------------------------------------

#if defined( _X360 )
	virtual void				ListUsedMaterials( void ) = 0;
	virtual HXUIFONT			OpenTrueTypeFont( const char *pFontname, int tall, int style ) = 0;
	virtual void				CloseTrueTypeFont( HXUIFONT hFont ) = 0;
	virtual bool				GetTrueTypeFontMetrics( HXUIFONT hFont, wchar_t wchFirst, wchar_t wchLast, XUIFontMetrics *pFontMetrics, XUICharMetrics *pCharMetrics ) = 0;
	// Render a sequence of characters and extract the data into a buffer
	// For each character, provide the width+height of the font texture subrect,
	// an offset to apply when rendering the glyph, and an offset into a buffer to receive the RGBA data
	virtual bool				GetTrueTypeGlyphs( HXUIFONT hFont, int numChars, wchar_t *pWch, int *pOffsetX, int *pOffsetY, int *pWidth, int *pHeight, unsigned char *pRGBA, int *pRGBAOffset ) = 0;
	virtual void				PersistDisplay() = 0;
	virtual void				*GetD3DDevice() = 0;
	virtual bool				OwnGPUResources( bool bEnable ) = 0;
#endif

	virtual void SpinPresent(uint nFrames) = 0;

	// -----------------------------------------------------------
	// Access the render contexts
	// -----------------------------------------------------------
	virtual IMatRenderContext *	GetRenderContext() = 0;

	virtual void				BeginUpdateLightmaps( void ) = 0;
	virtual void				EndUpdateLightmaps( void ) = 0;

	// -----------------------------------------------------------
	// Methods to force the material system into non-threaded, non-queued mode
	// -----------------------------------------------------------
	virtual MaterialLock_t		Lock() = 0;
	virtual void				Unlock( MaterialLock_t ) = 0;

	// Create a custom render context. Cannot be used to create MATERIAL_HARDWARE_CONTEXT
	virtual IMatRenderContext *CreateRenderContext( MaterialContextType_t type ) = 0;

	// Set a specified render context to be the global context for the thread. Returns the prior context.
	virtual IMatRenderContext *SetRenderContext( IMatRenderContext * ) = 0;

	virtual bool				SupportsCSAAMode( int nNumSamples, int nQualityLevel ) = 0;

	virtual void				RemoveModeChangeCallBack( ModeChangeCallbackFunc_t func ) = 0;

	// Finds or create a procedural material.
	virtual IMaterial *			FindProceduralMaterial( const char *pMaterialName, const char *pTextureGroupName, KeyValues *pVMTKeyValues ) = 0;

	virtual void				AddTextureAlias( const char *pAlias, const char *pRealName ) = 0;
	virtual void				RemoveTextureAlias( const char *pAlias ) = 0;

	// returns a lightmap page ID for this allocation, -1 if none available
	// frameID is a number that should be changed every frame to prevent locking any textures that are
	// being used to draw in the previous frame
	virtual int					AllocateDynamicLightmap( int lightmapSize[2], int *pOutOffsetIntoPage, int frameID ) = 0;

	virtual void				SetExcludedTextures( const char *pScriptName ) = 0;
	virtual void				UpdateExcludedTextures( void ) = 0;

	virtual bool				IsInFrame( ) const = 0;

	virtual void				CompactMemory() = 0;

	// todo: reconstruct GPUMemoryStats_t or whatever it's called
	virtual void				GetGPUMemoryStats(void* par1) = 0;

	// For sv_pure mode. The filesystem figures out which files the client needs to reload to be "pure" ala the server's preferences.
	virtual void				ReloadFilesInList( IFileList *pFilesToReload ) = 0;

	// Get information about the texture for texture management tools
	virtual bool				GetTextureInformation( char const *szTextureName, MaterialTextureInfo_t &info ) const = 0;

	// call this once the render targets are allocated permanently at the beginning of the game
	virtual void				FinishRenderTargetAllocation( void ) = 0;
	
	virtual void				ReEnableRenderTargetAllocation_IRealizeIfICallThisAllTexturesWillBeUnloadedAndLoadTimeWillSufferHorribly( void ) = 0;
	virtual	bool				AllowThreading( bool bAllow, int nServiceThread ) = 0;

	virtual bool				GetRecommendedVideoConfig( KeyValues *pKeyValues ) = 0;

	virtual IClientMaterialSystem*	GetClientMaterialSystemInterface() = 0;

	virtual bool				CanDownloadTextures() const = 0;
	virtual int					GetNumLightmapPages() const = 0;

	virtual IPaintMapTextureManager *RegisterPaintMapDataManager( IPaintMapDataManager *pDataManager ) = 0; //You supply an interface we can query for bits, it gives back an interface you can use to drive updates
	
	virtual void				BeginUpdatePaintmaps(void) = 0;
	virtual void				EndUpdatePaintmaps(void) = 0;
	virtual void				UpdatePaintmap(int par1, unsigned char* par2, int par3, Rect_t par4) = 0;

	// 0x00019030
	virtual ITexture* CreateNamedMultiRenderTargetTexture(char* param_1,
		int param_2, int param_3,
		RenderTargetSizeMode_t param_4,
		ImageFormat param_5, MaterialRenderTargetDepth_t param_6,
		uint param_7, uint param_8) = 0;

	virtual void Unused0() = 0;
	virtual void Unused1() = 0;
	virtual void Unused2() = 0;
	virtual void Unused3() = 0;
	virtual void Unused4() = 0;
	virtual void Unused5() = 0;
	virtual void Unused6() = 0;
	virtual void Unused7() = 0;
	virtual void Unused8() = 0;
	virtual void Unused9() = 0;

	virtual void Unused01() = 0;
	virtual void Unused11() = 0;
	virtual void Unused21() = 0;
	virtual void Unused31() = 0;
	virtual void Unused41() = 0;
	virtual void Unused51() = 0;
	virtual void Unused61() = 0;
	virtual void Unused71() = 0;
	virtual void Unused81() = 0;
	virtual void Unused91() = 0;

	virtual void Unused02() = 0;
	virtual void Unused12() = 0;
	virtual void Unused22() = 0;
	virtual void Unused32() = 0;
	virtual void Unused42() = 0;
	virtual void Unused52() = 0;
	virtual void Unused62() = 0;
	virtual void Unused72() = 0;
	virtual void Unused82() = 0;
	virtual void Unused92() = 0;

	virtual void Unused03() = 0;
	virtual void Unused13() = 0;
	virtual void Unused23() = 0;
	virtual void Unused33() = 0;
	virtual void Unused43() = 0;
	virtual void Unused53() = 0;
	virtual void Unused63() = 0;
	virtual void Unused73() = 0;
	virtual void Unused83() = 0;
	virtual void Unused93() = 0;

	virtual ITexture* GetPaintmapTexture(int param_1) = 0;
};

class IMeshOld;

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
abstract_class IMatRenderContextOld : public IRefCounted
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;

	virtual void				Flush(bool flushHardware = false) = 0;

	virtual void				BindLocalCubemap(ITexture* pTexture) = 0;

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture* pTexture) = 0;
	virtual ITexture*			GetRenderTarget(void) = 0;

	virtual void				GetRenderTargetDimensions(int& width, int& height) const = 0;

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial* material, void* proxyData = 0) = 0;
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) = 0;

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) = 0;

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char* data, ImageFormat dstFormat) = 0;

	// Sets lighting
	virtual void				SetLightingState(const MaterialLightingState_t& state) = 0;
	virtual void				SetLights(int nCount, const LightDesc_t* pLights) = 0;

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture* pTexture) = 0;

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) = 0;
	virtual ITexture*			GetFrameBufferCopyTexture(int textureIndex) = 0;	// 17 down, 0x44

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const& matrix) = 0;
	virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrix(VMatrix const& matrix) = 0;
	virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix* matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t* matrix) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;

	// The cull mode
	virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
	virtual void				FlipCullMode(void) = 0; //CW->CCW or CCW->CW, intended for mirror support where the view matrix is flipped horizontally

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) = 0;

	// Fog methods...
	virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual MaterialFogMode_t	GetFogMode(void) = 0;

	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const* rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const* rgb) = 0; // 50 down

	virtual void				GetFogColor(unsigned char* rgb) = 0;

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) = 0;

	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL, VertexStreamSpec_t* pStreamSpec = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual IMeshOld* GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial* pAutoBind = 0) = 0;

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char* pTextureBudgetGroup) = 0;
	virtual IIndexBuffer* CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char* pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer*) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer*) = 0;
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer* GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer* GetDynamicIndexBuffer() = 0;
	virtual void BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0; // 70 down

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;

	// Allows us to override the depth buffer setting of a material
	//virtual void	OverrideDepthEnable( bool bEnable, bool bDepthEnable ) = 0;
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable, bool p3 = false) = 0;

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char* pToken) = 0;

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;

	virtual void SetFlashlightMode(bool bEnable) = 0;

	virtual void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) = 0;

	// Gets the current height clip mode
	virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix& worldToView) = 0;

	virtual bool GetFlashlightMode() const = 0;

	// new 
	virtual bool IsCullingEnabledForSinglePassFlashlight() const = 0;
	virtual void EnableCullingForSinglePassFlashlight(bool par1) = 0;
	// end

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph* CreateMorph(MorphFormat_t format, const char* pDebugName) = 0;
	virtual void DestroyMorph(IMorph* pMorph) = 0;

	// Binds the morph data for use in rendering
	virtual void BindMorph(IMorph* pMorph) = 0;

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0; // 95

	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void*			LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void* pData) = 0;

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E*	LockRenderDataTyped(int nCount, const E* pSrcData = NULL);

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0; // 100 down

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void* pData) const = 0;

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, unsigned char* pBuffer, ImageFormat dstFormat, int nDstStride) = 0;

	// Gets the window size
	virtual void GetWindowSize(int& width, int& height) const = 0;

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle(
		IMaterial* pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void* pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) = 0;

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() = 0;

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) = 0;

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture* pLightmapTexture) = 0;

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = NULL) = 0;

	virtual void CopyTextureToRenderTargetEx(int par1, ITexture* par2, Rect_t* par3, Rect_t* par4) = 0;

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;

	// Sets the ambient light color // no longer in vtable
	//virtual void SetAmbientLightColor( float r, float g, float b ) = 0;
	void SetAmbientLightColor(float r, float g, float b) {}

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const& value) = 0;

	// stencil buffer operations.
	virtual void SetStencilState(const ShaderStencilState_t& state) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) = 0;

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) = 0;

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float* pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0; // 122

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) = 0;

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial* pMaterial) = 0; // 1f4
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) = 0;

	virtual IMesh* GetFlexMesh() = 0;

	virtual void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture, ITexture* pFlashlightDepthTexture) = 0;
	// removed from vtable
	//virtual void SetFlashlightStateExDeRef( const FlashlightState_t &state, ITexture *pFlashlightDepthTexture ) = 0;

	// Returns the currently bound local cubemap
	virtual ITexture* GetLocalCubemap() = 0;

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) = 0;

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float* fStart, float* fEnd, float* fFogZ) = 0;

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char* szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char* szName) = 0;

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(IMesh* pIndices) = 0;
	virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = NULL) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue* GetCallQueue() = 0;

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) = 0;

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector& scale) = 0;
	virtual Vector				GetToneMappingScaleLinear(void) = 0;

	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) = 0;

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;

	// Set scissor rect for rendering
	// renamed in vtable
	//virtual void				SetScissorRect( const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor ) = 0;
	virtual void				PushScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;
	virtual void				PopScissorRect() = 0; // new in vtable

	// Methods used to build the morph accumulator that is read from when HW morphing is enabled.
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;

	virtual void				PushDeformation(DeformationBase_t const* Deformation) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D* pTexCoord, IMorph* pMorph, int nVertex) = 0;

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0,	IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0 ) = 0;

	virtual void				FogMaxDensity(float flMaxDensity) = 0;

	virtual void AntiAliasingHint(int par1) = 0;

#if defined( _X360 )
	//Seems best to expose GPR allocation to scene rendering code. 128 total to split between vertex/pixel shaders (pixel will be set to 128 - vertex). Minimum value of 16. More GPR's = more threads.
	virtual void				PushVertexShaderGPRAllocation(int iVertexShaderCount = 64) = 0;
	virtual void				PopVertexShaderGPRAllocation(void) = 0;

	virtual void				FlushHiStencil() = 0;

	virtual void				Begin360ZPass(int nNumDynamicIndicesNeeded) = 0;
	virtual void				End360ZPass() = 0;
#endif

	virtual IMaterial* GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void* GetCurrentProxy() = 0;

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char* pName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char* pLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractiveLogoTexture(ITexture* param_1, float param_2, float param_3, float param_4, float param_5) = 0;

	virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, MaterialNonInteractiveMode_t mode) = 0;
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;

	// Flip culling state (swap CCW <-> CW)
	virtual void FlipCulling(bool bFlipCulling) = 0;

	virtual void SetTextureRenderingParameter(int parm_number, ITexture* pTexture) = 0;

	//only actually sets a bool that can be read in from shaders, doesn't do any of the legwork
	virtual void EnableSinglePassFlashlightMode(bool bEnable) = 0;
	virtual bool SinglePassFlashlightModeEnabled() = 0; // new

	// Draws instances with different meshes
	virtual void DrawInstances(int nInstanceCount, const MeshInstanceData_t* pInstance) = 0;

	// Allows us to override the color/alpha write settings of a material
	virtual void OverrideAlphaWriteEnable(bool bOverrideEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;

	// Subdivision surface interface
	virtual int GetSubDBufferWidth() = 0;
	virtual float* LockSubDBuffer(int nNumRows) = 0;
	virtual void UnlockSubDBuffer() = 0;

	// Update current frame's game time for the shader api.
	virtual void UpdateGameTime(float flTime) = 0;

	virtual void			PrintfVA(char* fmt, va_list vargs) = 0;
	virtual void			Printf(char* fmt, ...) = 0;
	virtual float			Knob(char* knobname, float* setvalue = NULL) = 0;

	virtual void SetRenderingPaint(bool par1) = 0;
	virtual ColorCorrectionHandle_t FindLookup(const char* pName) = 0; // 71 past GetMaxVerticesToRender

};

template< class E > inline E* IMatRenderContextOld::LockRenderDataTyped( int nCount, const E* pSrcData )
{
	int nSizeInBytes = nCount * sizeof(E);
	E *pDstData = (E*)LockRenderData( nSizeInBytes );
	if ( pSrcData && pDstData )
	{
		memcpy( pDstData, pSrcData, nSizeInBytes );
	}
	return pDstData;
}

abstract_class IMatRenderContext : public IRefCounted
{
public:
	virtual void				BeginRender() = 0;
	virtual void				EndRender() = 0;

	virtual void				Flush(bool flushHardware = false) = 0;

	virtual void				BindLocalCubemap(ITexture* pTexture) = 0;

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture* pTexture) = 0;
	virtual ITexture* GetRenderTarget(void) = 0;

	virtual void				GetRenderTargetDimensions(int& width, int& height) const = 0;

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial* material, void* proxyData = 0) = 0;
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) = 0;

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) = 0;

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) = 0;

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char* data, ImageFormat dstFormat) = 0;

	// Sets lighting
	virtual void				SetLightingState(const MaterialLightingState_t& state) = 0;
	virtual void				SetLights(int nCount, const LightDesc_t* pLights) = 0;

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) = 0;

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture* pTexture) = 0;

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) = 0;
	virtual ITexture* GetFrameBufferCopyTexture(int textureIndex) = 0;

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) = 0;
	virtual void				PushMatrix(void) = 0;
	virtual void				PopMatrix(void) = 0;
	virtual void				LoadMatrix(VMatrix const& matrix) = 0;
	virtual void				LoadMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrix(VMatrix const& matrix) = 0;
	virtual void				MultMatrix(matrix3x4_t const& matrix) = 0;
	virtual void				MultMatrixLocal(VMatrix const& matrix) = 0;
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix* matrix) = 0;
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t* matrix) = 0;
	virtual void				LoadIdentity(void) = 0;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0;
	virtual void				PickMatrix(int x, int y, int width, int height) = 0;
	virtual void				Rotate(float angle, float x, float y, float z) = 0;
	virtual void				Translate(float x, float y, float z) = 0;
	virtual void				Scale(float x, float y, float z) = 0;
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) = 0;
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;

	// The cull mode
	virtual void				CullMode(MaterialCullMode_t cullMode) = 0;
	virtual void				FlipCullMode(void) = 0; //CW->CCW or CCW->CW, intended for mirror support where the view matrix is flipped horizontally

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) = 0;
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) = 0;

	// Fog methods...
	virtual void				FogMode(MaterialFogMode_t fogMode) = 0;
	virtual void				FogStart(float fStart) = 0;
	virtual void				FogEnd(float fEnd) = 0;
	virtual void				SetFogZ(float fogZ) = 0;
	virtual MaterialFogMode_t	GetFogMode(void) = 0;

	virtual void				FogColor3f(float r, float g, float b) = 0;
	virtual void				FogColor3fv(float const* rgb) = 0;
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void				FogColor3ubv(unsigned char const* rgb) = 0;

	virtual void				GetFogColor(unsigned char* rgb) = 0;

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) = 0;

	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL, VertexStreamSpec_t* pStreamSpec = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual IMesh* GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial* pAutoBind = 0) = 0;

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char* pTextureBudgetGroup) = 0;
	virtual IIndexBuffer* CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char* pTextureBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer*) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer*) = 0;
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer* GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer* GetDynamicIndexBuffer() = 0;
	virtual void BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) = 0;
	virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) = 0;
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) = 0;
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) = 0;
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) = 0;
	virtual void ClearSelectionNames() = 0;
	virtual void LoadSelectionName(int name) = 0;
	virtual void PushSelectionName(int name) = 0;
	virtual void PopSelectionName() = 0;

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) = 0;
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) = 0;

	// Allows us to override the depth buffer setting of a material
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) = 0;

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) = 0;

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char* pToken) = 0;

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) = 0;

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) = 0;
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t) = 0;

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t) = 0;

	virtual void SetFlashlightMode(bool bEnable) = 0;

	virtual void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) = 0;

	// Gets the current height clip mode
	virtual MaterialHeightClipMode_t GetHeightClipMode() = 0;

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) = 0;

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) = 0;
	virtual void	UserClipTransform(const VMatrix& worldToView) = 0;

	virtual bool GetFlashlightMode() const = 0;

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t) = 0;

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph* CreateMorph(MorphFormat_t format, const char* pDebugName) = 0;
	virtual void DestroyMorph(IMorph* pMorph) = 0;

	// Binds the morph data for use in rendering
	virtual void BindMorph(IMorph* pMorph) = 0;

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) = 0;

	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void* LockRenderData(int nSizeInBytes) = 0;
	virtual void			UnlockRenderData(void* pData) = 0;

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E* LockRenderDataTyped(int nCount, const E* pSrcData = NULL);

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() = 0;
	virtual void			ReleaseRenderData() = 0;

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void* pData) const = 0;

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, unsigned char* pBuffer, ImageFormat dstFormat, int nDstStride) = 0;

	// Gets the window size
	virtual void GetWindowSize(int& width, int& height) const = 0;

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle(
		IMaterial* pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void* pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) = 0;

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) = 0;

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() = 0;

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) = 0;

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) = 0;

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture* pLightmapTexture) = 0;

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = NULL) = 0;

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) = 0;

	// Sets the ambient light color
	virtual void SetAmbientLightColor(float r, float g, float b) = 0;

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) = 0;
	virtual void SetIntRenderingParameter(int parm_number, int value) = 0;
	virtual void SetVectorRenderingParameter(int parm_number, Vector const& value) = 0;

	// stencil buffer operations.
	virtual void SetStencilState(const ShaderStencilState_t& state) = 0;
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) = 0;

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) = 0;

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float* pPlane) = 0;
	virtual void PopCustomClipPlane(void) = 0;

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) = 0;

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial* pMaterial) = 0;
	virtual int GetMaxIndicesToRender() = 0;
	virtual void DisableAllLocalLights() = 0;
	virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) = 0;

	virtual IMesh* GetFlexMesh() = 0;

	virtual void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture, ITexture* pFlashlightDepthTexture) = 0;
	virtual void SetFlashlightStateExDeRef(const FlashlightState_t& state, ITexture* pFlashlightDepthTexture) = 0;

	// Returns the currently bound local cubemap
	virtual ITexture* GetLocalCubemap() = 0;

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) = 0;

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) = 0;

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float* fStart, float* fEnd, float* fFogZ) = 0;

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char* szName) = 0;
	virtual void EndPIXEvent() = 0;
	virtual void SetPIXMarker(unsigned long color, const char* szName) = 0;

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(IMesh* pIndices) = 0;
	virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = NULL) = 0;
	virtual void DrawBatch(int firstIndex, int numIndices) = 0;
	virtual void EndBatch() = 0;

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue* GetCallQueue() = 0;

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) = 0;
	virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) = 0;

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector& scale) = 0;
	virtual Vector				GetToneMappingScaleLinear(void) = 0;

	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) = 0;

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) = 0;

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) = 0;

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) = 0;

	// Methods used to build the morph accumulator that is read from when HW morphing is enabled.
	virtual void				BeginMorphAccumulation() = 0;
	virtual void				EndMorphAccumulation() = 0;
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) = 0;

	virtual void				PushDeformation(DeformationBase_t const* Deformation) = 0;
	virtual void				PopDeformation() = 0;
	virtual int					GetNumActiveDeformations() const = 0;

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D* pTexCoord, IMorph* pMorph, int nVertex) = 0;

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0,	IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) = 0;

	virtual void				FogMaxDensity(float flMaxDensity) = 0;

#if defined( _X360 )
	//Seems best to expose GPR allocation to scene rendering code. 128 total to split between vertex/pixel shaders (pixel will be set to 128 - vertex). Minimum value of 16. More GPR's = more threads.
	virtual void				PushVertexShaderGPRAllocation(int iVertexShaderCount = 64) = 0;
	virtual void				PopVertexShaderGPRAllocation(void) = 0;

	virtual void				FlushHiStencil() = 0;

	virtual void				Begin360ZPass(int nNumDynamicIndicesNeeded) = 0;
	virtual void				End360ZPass() = 0;
#endif

	virtual IMaterial* GetCurrentMaterial() = 0;
	virtual int  GetCurrentNumBones() const = 0;
	virtual void* GetCurrentProxy() = 0;

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) = 0;
	virtual ColorCorrectionHandle_t AddLookup(const char* pName) = 0;
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char* pLookupName) = 0;
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) = 0;
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) = 0;
	virtual void ResetLookupWeights() = 0;
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) = 0;

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) = 0;

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) = 0;
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, MaterialNonInteractiveMode_t mode) = 0;
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;

	// Flip culling state (swap CCW <-> CW)
	virtual void FlipCulling(bool bFlipCulling) = 0;

	virtual void SetTextureRenderingParameter(int parm_number, ITexture* pTexture) = 0;

	//only actually sets a bool that can be read in from shaders, doesn't do any of the legwork
	virtual void EnableSinglePassFlashlightMode(bool bEnable) = 0;

	// Draws instances with different meshes
	virtual void DrawInstances(int nInstanceCount, const MeshInstanceData_t* pInstance) = 0;

	// Allows us to override the color/alpha write settings of a material
	virtual void OverrideAlphaWriteEnable(bool bOverrideEnable, bool bAlphaWriteEnable) = 0;
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) = 0;

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) = 0;

	// Subdivision surface interface
	virtual int GetSubDBufferWidth() = 0;
	virtual float* LockSubDBuffer(int nNumRows) = 0;
	virtual void UnlockSubDBuffer() = 0;

	// Update current frame's game time for the shader api.
	virtual void UpdateGameTime(float flTime) = 0;

	virtual void			PrintfVA(char* fmt, va_list vargs) = 0;
	virtual void			Printf(char* fmt, ...) = 0;
	virtual float			Knob(char* knobname, float* setvalue = NULL) = 0;
};

// new proxy type interface for render context to work right with particles lib
class CMatRenderContextNew : public CRefCounted1<IMatRenderContext> {
protected:
	friend class CParticleSystemMgrPtr;

	IMatRenderContextOld* s_pOldRenderContext;
public:

	// these 2 are for refcount stuff
	//virtual void Unused1() {}
	//virtual void Unused2() {}

	virtual void				BeginRender() { s_pOldRenderContext->BeginRender(); }
	virtual void				EndRender() { s_pOldRenderContext->EndRender(); }

	virtual void				Flush(bool flushHardware = false) { s_pOldRenderContext->Flush(flushHardware); }

	virtual void				BindLocalCubemap(ITexture* pTexture) { s_pOldRenderContext->BindLocalCubemap(pTexture); }

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture* pTexture) { s_pOldRenderContext->SetRenderTarget(pTexture); }
	virtual ITexture* GetRenderTarget(void) { return s_pOldRenderContext->GetRenderTarget(); }

	virtual void				GetRenderTargetDimensions(int& width, int& height) const { s_pOldRenderContext->GetRenderTargetDimensions(width, height); }

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial* material, void* proxyData = 0) { s_pOldRenderContext->Bind(material, proxyData); }
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) { s_pOldRenderContext->BindLightmapPage(lightmapPageID); }

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) { s_pOldRenderContext->DepthRange(zNear, zFar); }

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) { s_pOldRenderContext->ClearBuffers(bClearColor, bClearDepth, bClearStencil); }

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char* data, ImageFormat dstFormat) { s_pOldRenderContext->ReadPixels(x, y, width, height, data, dstFormat); }

	// Sets lighting
	virtual void				SetLightingState(const MaterialLightingState_t& state) { s_pOldRenderContext->SetLightingState(state); }
	virtual void				SetLights(int nCount, const LightDesc_t* pLights) { s_pOldRenderContext->SetLights(nCount, pLights); }

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) { s_pOldRenderContext->SetAmbientLightCube(cube); }

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture* pTexture) { s_pOldRenderContext->CopyRenderTargetToTexture(pTexture); }

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) { s_pOldRenderContext->SetFrameBufferCopyTexture(pTexture, textureIndex); }
	virtual ITexture* GetFrameBufferCopyTexture(int textureIndex) { return s_pOldRenderContext->GetFrameBufferCopyTexture(textureIndex); }

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) { s_pOldRenderContext->MatrixMode(matrixMode); }
	virtual void				PushMatrix(void) { s_pOldRenderContext->PushMatrix(); }
	virtual void				PopMatrix(void) { s_pOldRenderContext->PopMatrix(); }
	virtual void				LoadMatrix(VMatrix const& matrix) { s_pOldRenderContext->LoadMatrix(matrix); }
	virtual void				LoadMatrix(matrix3x4_t const& matrix) { s_pOldRenderContext->LoadMatrix(matrix); }
	virtual void				MultMatrix(VMatrix const& matrix) { s_pOldRenderContext->MultMatrix(matrix); }
	virtual void				MultMatrix(matrix3x4_t const& matrix) { s_pOldRenderContext->MultMatrix(matrix); }
	virtual void				MultMatrixLocal(VMatrix const& matrix) { s_pOldRenderContext->MultMatrixLocal(matrix); }
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) { s_pOldRenderContext->MultMatrixLocal(matrix); }
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix* matrix) { s_pOldRenderContext->GetMatrix(matrixMode, matrix); }
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t* matrix) { s_pOldRenderContext->GetMatrix(matrixMode, matrix); }
	virtual void				LoadIdentity(void) { s_pOldRenderContext->LoadIdentity(); }
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) { s_pOldRenderContext->Ortho(left, top, right, bottom, zNear, zFar); }
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) { s_pOldRenderContext->PerspectiveX(fovx, aspect, zNear, zFar); }
	virtual void				PickMatrix(int x, int y, int width, int height) { s_pOldRenderContext->PickMatrix(x, y, width, height); }
	virtual void				Rotate(float angle, float x, float y, float z) { s_pOldRenderContext->Rotate(angle, x, y, z); }
	virtual void				Translate(float x, float y, float z) { s_pOldRenderContext->Translate(x, y, z); }
	virtual void				Scale(float x, float y, float z) { s_pOldRenderContext->Scale(x, y, z); }
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) { s_pOldRenderContext->Viewport(x, y, width, height); }
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const { s_pOldRenderContext->GetViewport(x, y, width, height); }

	// The cull mode
	virtual void				CullMode(MaterialCullMode_t cullMode) { s_pOldRenderContext->CullMode(cullMode); }
	virtual void				FlipCullMode(void) { s_pOldRenderContext->FlipCullMode(); } //CW->CCW or CCW->CW, intended for mirror support where the view matrix is flipped horizontally

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) { s_pOldRenderContext->SetHeightClipMode(nHeightClipMode); }
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) { s_pOldRenderContext->SetHeightClipZ(z); }

	// Fog methods...
	virtual void				FogMode(MaterialFogMode_t fogMode) { s_pOldRenderContext->FogMode(fogMode); }
	virtual void				FogStart(float fStart) { s_pOldRenderContext->FogStart(fStart); }
	virtual void				FogEnd(float fEnd) { s_pOldRenderContext->FogEnd(fEnd); }
	virtual void				SetFogZ(float fogZ) { s_pOldRenderContext->SetFogZ(fogZ); }
	virtual MaterialFogMode_t	GetFogMode(void) { return s_pOldRenderContext->GetFogMode(); }

	virtual void				FogColor3f(float r, float g, float b) { s_pOldRenderContext->FogColor3f(r, g, b); }
	virtual void				FogColor3fv(float const* rgb) { s_pOldRenderContext->FogColor3fv(rgb); }
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) { s_pOldRenderContext->FogColor3ub(r, g, b); }
	virtual void				FogColor3ubv(unsigned char const* rgb) { s_pOldRenderContext->FogColor3ubv(rgb); }

	virtual void				GetFogColor(unsigned char* rgb) { s_pOldRenderContext->GetFogColor(rgb); }

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) { s_pOldRenderContext->SetNumBoneWeights(numBones); }

	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL, VertexStreamSpec_t* pStreamSpec = NULL) { return s_pOldRenderContext->CreateStaticMesh(fmt, pTextureBudgetGroup, pMaterial, pStreamSpec); }
	virtual void DestroyStaticMesh(IMesh* mesh) { s_pOldRenderContext->DestroyStaticMesh(mesh); }

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual IMesh* GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial* pAutoBind = 0);/* {

		IMeshOld* pOldMesh = s_pOldRenderContext->GetDynamicMesh(buffered, pVertexOverride, pIndexOverride, pAutoBind);

		return new CMeshPtr(pOldMesh);
	}*/

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char* pTextureBudgetGroup) { return s_pOldRenderContext->CreateStaticVertexBuffer(fmt, nVertexCount, pTextureBudgetGroup); }
	virtual IIndexBuffer* CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char* pTextureBudgetGroup) { return s_pOldRenderContext->CreateStaticIndexBuffer(fmt, nIndexCount, pTextureBudgetGroup); }
	virtual void DestroyVertexBuffer(IVertexBuffer* buf) { s_pOldRenderContext->DestroyVertexBuffer(buf); }
	virtual void DestroyIndexBuffer(IIndexBuffer* buf) { s_pOldRenderContext->DestroyIndexBuffer(buf); }

	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer* GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) { return s_pOldRenderContext->GetDynamicVertexBuffer(streamID, vertexFormat, bBuffered); }
	virtual IIndexBuffer* GetDynamicIndexBuffer() { return s_pOldRenderContext->GetDynamicIndexBuffer(); }
	virtual void BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) { s_pOldRenderContext->BindVertexBuffer(streamID, pVertexBuffer, nOffsetInBytes, nFirstVertex, nVertexCount, fmt, nRepetitions); }
	virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) { s_pOldRenderContext->BindIndexBuffer(pIndexBuffer, nOffsetInBytes); }
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) { s_pOldRenderContext->Draw(primitiveType, firstIndex, numIndices); }
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) { return s_pOldRenderContext->SelectionMode(selectionMode); }
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) { s_pOldRenderContext->SelectionBuffer(pBuffer, size); }
	virtual void ClearSelectionNames() { s_pOldRenderContext->ClearSelectionNames(); }
	virtual void LoadSelectionName(int name) { s_pOldRenderContext->LoadSelectionName(name); }
	virtual void PushSelectionName(int name) { s_pOldRenderContext->PushSelectionName(name); }
	virtual void PopSelectionName() { s_pOldRenderContext->PopSelectionName(); }

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) { s_pOldRenderContext->ClearColor3ub(r, g, b); }
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { s_pOldRenderContext->ClearColor4ub(r, g, b, a); }

	// Allows us to override the depth buffer setting of a material
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) { s_pOldRenderContext->OverrideDepthEnable(bEnable, bDepthEnable); }

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) { s_pOldRenderContext->DrawScreenSpaceQuad(pMaterial); }

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char* pToken) { s_pOldRenderContext->SyncToken(pToken); }

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) { return s_pOldRenderContext->ComputePixelWidthOfSphere(origin, flRadius); }

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) { return s_pOldRenderContext->CreateOcclusionQueryObject(); }
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t t) { s_pOldRenderContext->DestroyOcclusionQueryObject(t); }

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t t) { s_pOldRenderContext->BeginOcclusionQueryDrawing(t); }
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t t) { s_pOldRenderContext->EndOcclusionQueryDrawing(t); }

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t t) { return s_pOldRenderContext->OcclusionQuery_GetNumPixelsRendered(t); }

	virtual void SetFlashlightMode(bool bEnable) { s_pOldRenderContext->SetFlashlightMode(bEnable); }

	virtual void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) { s_pOldRenderContext->SetFlashlightState(state, worldToTexture); }

	// Gets the current height clip mode
	virtual MaterialHeightClipMode_t GetHeightClipMode() { return s_pOldRenderContext->GetHeightClipMode(); }

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) { return s_pOldRenderContext->ComputePixelDiameterOfSphere(vecAbsOrigin, flRadius); }

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) { s_pOldRenderContext->EnableUserClipTransformOverride(bEnable); }
	virtual void	UserClipTransform(const VMatrix& worldToView) { s_pOldRenderContext->UserClipTransform(worldToView); }

	virtual bool GetFlashlightMode() const { return s_pOldRenderContext->GetFlashlightMode(); }

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t t) { s_pOldRenderContext->ResetOcclusionQueryObject(t); }

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph* CreateMorph(MorphFormat_t format, const char* pDebugName) { return s_pOldRenderContext->CreateMorph(format, pDebugName); }
	virtual void DestroyMorph(IMorph* pMorph) { s_pOldRenderContext->DestroyMorph(pMorph); }

	// Binds the morph data for use in rendering
	virtual void BindMorph(IMorph* pMorph) { s_pOldRenderContext->BindMorph(pMorph); }

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) { s_pOldRenderContext->SetFlexWeights(nFirstWeight, nCount, pWeights); }

	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void* LockRenderData(int nSizeInBytes) { return s_pOldRenderContext->LockRenderData(nSizeInBytes); }
	virtual void			UnlockRenderData(void* pData) { s_pOldRenderContext->UnlockRenderData(pData); }

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E* LockRenderDataTyped(int nCount, const E* pSrcData = NULL);

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() { s_pOldRenderContext->AddRefRenderData(); }
	virtual void			ReleaseRenderData() { s_pOldRenderContext->ReleaseRenderData(); }

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void* pData) const { return s_pOldRenderContext->IsRenderData(pData); }

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, unsigned char* pBuffer, ImageFormat dstFormat, int nDstStride) { s_pOldRenderContext->ReadPixelsAndStretch(pSrcRect, pDstRect, pBuffer, dstFormat, nDstStride); }

	// Gets the window size
	virtual void GetWindowSize(int& width, int& height) const { s_pOldRenderContext->GetWindowSize(width, height); }

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle(
		IMaterial* pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void* pClientRenderable = NULL,
		int nXDice = 1,
		int nYDice = 1) {
		s_pOldRenderContext->DrawScreenSpaceRectangle(pMaterial, destx, desty, width, height, src_texture_x0, src_texture_y0, src_texture_x1, src_texture_y1, src_texture_width, src_texture_height, pClientRenderable, nXDice, nYDice);
	}

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) { s_pOldRenderContext->LoadBoneMatrix(boneIndex, matrix); }

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() { s_pOldRenderContext->PushRenderTargetAndViewport(); }

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture) { s_pOldRenderContext->PushRenderTargetAndViewport(pTexture); }

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) { s_pOldRenderContext->PushRenderTargetAndViewport(pTexture, nViewX, nViewY, nViewW, nViewH); }

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) { s_pOldRenderContext->PushRenderTargetAndViewport(pTexture, pDepthTexture, nViewX, nViewY, nViewW, nViewH); }

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) { s_pOldRenderContext->PopRenderTargetAndViewport(); }

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture* pLightmapTexture) { s_pOldRenderContext->BindLightmapTexture(pLightmapTexture); }

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = NULL) { s_pOldRenderContext->CopyRenderTargetToTextureEx(pTexture, nRenderTargetID, pSrcRect, pDstRect); }

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) { s_pOldRenderContext->PerspectiveOffCenterX(fovx, aspect, zNear, zFar, bottom, top, left, right); }

	// Sets the ambient light color
	virtual void SetAmbientLightColor(float r, float g, float b) { s_pOldRenderContext->SetAmbientLightColor( r,  g,  b); }

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) { s_pOldRenderContext->SetFloatRenderingParameter(parm_number, value); }
	virtual void SetIntRenderingParameter(int parm_number, int value) { s_pOldRenderContext->SetIntRenderingParameter(parm_number, value); }
	virtual void SetVectorRenderingParameter(int parm_number, Vector const& value) { s_pOldRenderContext->SetVectorRenderingParameter(parm_number, value); }

	// stencil buffer operations.
	virtual void SetStencilState(const ShaderStencilState_t& state) { s_pOldRenderContext->SetStencilState(state); }
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) { s_pOldRenderContext->ClearStencilBufferRectangle(xmin, ymin, xmax, ymax, value); }

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) { s_pOldRenderContext->SetRenderTargetEx(nRenderTargetID, pTexture); }

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float* pPlane) { s_pOldRenderContext->PushCustomClipPlane(pPlane); }
	virtual void PopCustomClipPlane(void) { s_pOldRenderContext->PopCustomClipPlane(); }

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) { s_pOldRenderContext->GetMaxToRender(pMesh, bMaxUntilFlush, pMaxVerts, pMaxIndices); }

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial* pMaterial) { return s_pOldRenderContext->GetMaxVerticesToRender(pMaterial); }
	virtual int GetMaxIndicesToRender() { return s_pOldRenderContext->GetMaxIndicesToRender(); }
	virtual void DisableAllLocalLights() { s_pOldRenderContext->DisableAllLocalLights(); }
	virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) { return s_pOldRenderContext->CompareMaterialCombos(pMaterial1, pMaterial2, lightMapID1, lightMapID2); }

	virtual IMesh* GetFlexMesh() { return s_pOldRenderContext->GetFlexMesh(); }

	virtual void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture, ITexture* pFlashlightDepthTexture) { s_pOldRenderContext->SetFlashlightStateEx(state, worldToTexture, pFlashlightDepthTexture); }
	virtual void SetFlashlightStateExDeRef(const FlashlightState_t& state, ITexture* pFlashlightDepthTexture) { /*s_pOldRenderContext->SetFlashlightStateExDeRef()*/ }

	// Returns the currently bound local cubemap
	virtual ITexture* GetLocalCubemap() { return s_pOldRenderContext->GetLocalCubemap(); }

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) { s_pOldRenderContext->ClearBuffersObeyStencil(bClearColor, bClearDepth); }

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) { return s_pOldRenderContext->EnableClipping(bEnable); }

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float* fStart, float* fEnd, float* fFogZ) { s_pOldRenderContext->GetFogDistances(fStart, fEnd, fFogZ); }

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char* szName) { s_pOldRenderContext->BeginPIXEvent(color, szName); }
	virtual void EndPIXEvent() { s_pOldRenderContext->EndPIXEvent(); }
	virtual void SetPIXMarker(unsigned long color, const char* szName) { s_pOldRenderContext->SetPIXMarker(color, szName); }

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(IMesh* pIndices) { s_pOldRenderContext->BeginBatch(pIndices); }
	virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = NULL) { s_pOldRenderContext->BindBatch(pVertices, pAutoBind); }
	virtual void DrawBatch(int firstIndex, int numIndices) { s_pOldRenderContext->DrawBatch(firstIndex, numIndices); }
	virtual void EndBatch() { s_pOldRenderContext->EndBatch(); }

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue* GetCallQueue() { return s_pOldRenderContext->GetCallQueue(); }

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) { s_pOldRenderContext->GetWorldSpaceCameraPosition(pCameraPos); }
	virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) { s_pOldRenderContext->GetWorldSpaceCameraVectors(pVecForward, pVecRight, pVecUp); }

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector& scale) { s_pOldRenderContext->SetToneMappingScaleLinear(scale); }
	virtual Vector				GetToneMappingScaleLinear(void) { return s_pOldRenderContext->GetToneMappingScaleLinear(); }

	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) { s_pOldRenderContext->SetShadowDepthBiasFactors(fSlopeScaleDepthBias, fDepthBias); }

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) { s_pOldRenderContext->PerformFullScreenStencilOperation(); }

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) { s_pOldRenderContext->SetLightingOrigin(vLightingOrigin); }

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) { /*s_pOldRenderContext->SetScissorRect()*/ }

	// Methods used to build the morph accumulator that is read from when HW morphing is enabled.
	virtual void				BeginMorphAccumulation() { s_pOldRenderContext->BeginMorphAccumulation(); }
	virtual void				EndMorphAccumulation() { s_pOldRenderContext->EndMorphAccumulation(); }
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) { s_pOldRenderContext->AccumulateMorph(pMorph, nMorphCount, pWeights); }

	virtual void				PushDeformation(DeformationBase_t const* Deformation) { s_pOldRenderContext->PushDeformation(Deformation); }
	virtual void				PopDeformation() { s_pOldRenderContext->PopDeformation(); }
	virtual int					GetNumActiveDeformations() const { return s_pOldRenderContext->GetNumActiveDeformations(); }

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D* pTexCoord, IMorph* pMorph, int nVertex) { return s_pOldRenderContext->GetMorphAccumulatorTexCoord(pTexCoord, pMorph, nVertex); }

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0, IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) {
		return s_pOldRenderContext->GetDynamicMeshEx(vertexFormat, bBuffered, pVertexOverride, pIndexOverride, pAutoBind);
	}

	virtual void				FogMaxDensity(float flMaxDensity) { s_pOldRenderContext->FogMaxDensity(flMaxDensity); }

#if defined( _X360 )
	//Seems best to expose GPR allocation to scene rendering code. 128 total to split between vertex/pixel shaders (pixel will be set to 128 - vertex). Minimum value of 16. More GPR's = more threads.
	virtual void				PushVertexShaderGPRAllocation(int iVertexShaderCount = 64) { s_pOldRenderContext-> }
	virtual void				PopVertexShaderGPRAllocation(void) { s_pOldRenderContext-> }

	virtual void				FlushHiStencil() { s_pOldRenderContext-> }

	virtual void				Begin360ZPass(int nNumDynamicIndicesNeeded) { s_pOldRenderContext-> }
	virtual void				End360ZPass() { s_pOldRenderContext-> }
#endif

	virtual IMaterial* GetCurrentMaterial() { return s_pOldRenderContext->GetCurrentMaterial(); }
	virtual int  GetCurrentNumBones() const { return s_pOldRenderContext->GetCurrentNumBones(); }
	virtual void* GetCurrentProxy() { return s_pOldRenderContext->GetCurrentProxy(); }

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) { s_pOldRenderContext->EnableColorCorrection(bEnable); }
	virtual ColorCorrectionHandle_t AddLookup(const char* pName) { return s_pOldRenderContext->AddLookup(pName); }
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) { return s_pOldRenderContext->RemoveLookup(handle); }
	virtual void LockLookup(ColorCorrectionHandle_t handle) { s_pOldRenderContext->LockLookup(handle); }
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char* pLookupName) { s_pOldRenderContext->LoadLookup(handle, pLookupName); }
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) { s_pOldRenderContext->UnlockLookup(handle); }
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) { s_pOldRenderContext->SetLookupWeight(handle, flWeight); }
	virtual void ResetLookupWeights() { s_pOldRenderContext->ResetLookupWeights(); }
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) { s_pOldRenderContext->SetResetable(handle, bResetable); }

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) { s_pOldRenderContext->SetFullScreenDepthTextureValidityFlag(bIsValid); }

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) { s_pOldRenderContext->SetNonInteractivePacifierTexture(pTexture, flNormalizedX, flNormalizedY, flNormalizedSize); }
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, MaterialNonInteractiveMode_t mode) { s_pOldRenderContext->SetNonInteractiveTempFullscreenBuffer(pTexture, mode); }
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) { s_pOldRenderContext->EnableNonInteractiveMode(mode); }
	virtual void RefreshFrontBufferNonInteractive() { s_pOldRenderContext->RefreshFrontBufferNonInteractive(); }

	// Flip culling state (swap CCW <-> CW)
	virtual void FlipCulling(bool bFlipCulling) { s_pOldRenderContext->FlipCulling(bFlipCulling); }

	virtual void SetTextureRenderingParameter(int parm_number, ITexture* pTexture) { s_pOldRenderContext->SetTextureRenderingParameter(parm_number, pTexture); }

	//only actually sets a bool that can be read in from shaders, doesn't do any of the legwork
	virtual void EnableSinglePassFlashlightMode(bool bEnable) { s_pOldRenderContext->EnableSinglePassFlashlightMode(bEnable); }

	// Draws instances with different meshes
	virtual void DrawInstances(int nInstanceCount, const MeshInstanceData_t* pInstance) { s_pOldRenderContext->DrawInstances(nInstanceCount, pInstance); }

	// Allows us to override the color/alpha write settings of a material
	virtual void OverrideAlphaWriteEnable(bool bOverrideEnable, bool bAlphaWriteEnable) { s_pOldRenderContext->OverrideAlphaWriteEnable(bOverrideEnable, bAlphaWriteEnable); }
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) { s_pOldRenderContext->OverrideColorWriteEnable(bOverrideEnable, bColorWriteEnable); }

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) { s_pOldRenderContext->ClearBuffersObeyStencilEx(bClearColor, bClearAlpha, bClearDepth); }

	// Subdivision surface interface
	virtual int GetSubDBufferWidth() { return s_pOldRenderContext->GetSubDBufferWidth(); }
	virtual float* LockSubDBuffer(int nNumRows) { return s_pOldRenderContext->LockSubDBuffer(nNumRows); }
	virtual void UnlockSubDBuffer() { s_pOldRenderContext->UnlockSubDBuffer(); }

	// Update current frame's game time for the shader api.
	virtual void UpdateGameTime(float flTime) { s_pOldRenderContext->UpdateGameTime(flTime); }

	virtual void			PrintfVA(char* fmt, va_list vargs) {}
	virtual void			Printf(char* fmt, ...) {}
	virtual float			Knob(char* knobname, float* setvalue = NULL) { return 0; }

	CMatRenderContextNew() {}

};

template< class E > inline E* IMatRenderContext::LockRenderDataTyped(int nCount, const E* pSrcData)
{
	int nSizeInBytes = nCount * sizeof(E);
	E* pDstData = (E*)LockRenderData(nSizeInBytes);
	if (pSrcData && pDstData)
	{
		memcpy(pDstData, pSrcData, nSizeInBytes);
	}
	return pDstData;
}

//-----------------------------------------------------------------------------
// Utility class for addreffing/releasing render data (prevents freeing on single core)
//-----------------------------------------------------------------------------
class CMatRenderDataReference
{
public:
	CMatRenderDataReference();
	CMatRenderDataReference( IMatRenderContext* pRenderContext );
	~CMatRenderDataReference();
	void Lock( IMatRenderContext *pRenderContext );
	void Release();

private:
	IMatRenderContext *m_pRenderContext;
};


inline CMatRenderDataReference::CMatRenderDataReference()
{
	m_pRenderContext = NULL;
}

inline CMatRenderDataReference::CMatRenderDataReference( IMatRenderContext* pRenderContext )
{
	m_pRenderContext = NULL;
	Lock( pRenderContext );
}

inline CMatRenderDataReference::~CMatRenderDataReference()
{
	Release();
}

inline void CMatRenderDataReference::Lock( IMatRenderContext* pRenderContext )
{
	if ( !m_pRenderContext )
	{
		m_pRenderContext = pRenderContext;
		m_pRenderContext->AddRefRenderData( );
	}
}

inline void CMatRenderDataReference::Release()
{
	if ( m_pRenderContext )
	{
		m_pRenderContext->ReleaseRenderData( );
		m_pRenderContext = NULL;
	}
}


//-----------------------------------------------------------------------------
// Utility class for locking/unlocking render data
//-----------------------------------------------------------------------------
template< typename E > 
class CMatRenderData
{
public:
	CMatRenderData( IMatRenderContext* pRenderContext );
	CMatRenderData( IMatRenderContext* pRenderContext, int nCount, const E *pSrcData = NULL );
	~CMatRenderData();
	E* Lock( int nCount, const E* pSrcData = NULL ); 
	void Release();
	bool IsValid() const;
	const E* Base() const;
	E* Base();
	const E& operator[]( int i ) const;
	E& operator[]( int i );

private:
	IMatRenderContext* m_pRenderContext;
	E *m_pRenderData;
	int m_nCount;
	bool m_bNeedsUnlock;
};

template< typename E >
inline CMatRenderData<E>::CMatRenderData( IMatRenderContext* pRenderContext )
{
	m_pRenderContext = pRenderContext;
	m_nCount = 0;
	m_pRenderData = 0;
	m_bNeedsUnlock = false;
}

template< typename E >
inline CMatRenderData<E>::CMatRenderData( IMatRenderContext* pRenderContext, int nCount, const E* pSrcData )
{
	m_pRenderContext = pRenderContext;
	m_nCount = 0;
	m_pRenderData = 0;
	m_bNeedsUnlock = false;
	Lock( nCount, pSrcData );
}

template< typename E >
inline CMatRenderData<E>::~CMatRenderData()
{
	Release();
}

template< typename E >
inline bool CMatRenderData<E>::IsValid() const
{
	return m_pRenderData != NULL;
}

template< typename E >
inline E* CMatRenderData<E>::Lock( int nCount, const E* pSrcData )
{
	m_nCount = nCount;
	if ( pSrcData && m_pRenderContext->IsRenderData( pSrcData ) )
	{
		// Yes, we're const-casting away, but that should be ok since 
		// the src data is render data
		m_pRenderData = const_cast<E*>( pSrcData );
		m_pRenderContext->AddRefRenderData();
		m_bNeedsUnlock = false;
		return m_pRenderData;
	}
	m_pRenderData = m_pRenderContext->LockRenderDataTyped<E>( nCount, pSrcData );
	m_bNeedsUnlock = true;
	return m_pRenderData;
}

template< typename E >
inline void CMatRenderData<E>::Release()
{
	if ( m_pRenderContext && m_pRenderData )
	{
		if ( m_bNeedsUnlock )
		{
			m_pRenderContext->UnlockRenderData( m_pRenderData );
		}
		else
		{
			m_pRenderContext->ReleaseRenderData();
		}
	}
	m_pRenderData = NULL;
	m_nCount = 0;
	m_bNeedsUnlock = false;
}

template< typename E >
inline E* CMatRenderData<E>::Base()
{
	return m_pRenderData;
}

template< typename E >
inline const E* CMatRenderData<E>::Base() const
{
	return m_pRenderData;
}

template< typename E >
inline E& CMatRenderData<E>::operator[]( int i )
{
	Assert( ( i >= 0 ) && ( i < m_nCount ) );
	return m_pRenderData[i];
}

template< typename E >
inline const E& CMatRenderData<E>::operator[]( int i ) const
{
	Assert( ( i >= 0 ) && ( i < m_nCount ) );
	return m_pRenderData[i];
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Helper class for begin/end of pix event via constructor/destructor 
//-----------------------------------------------------------------------------
#define PIX_VALVE_ORANGE	0xFFF5940F

class PIXEvent
{
public:
	PIXEvent( IMatRenderContext *pRenderContext, const char *szName, unsigned long color = PIX_VALVE_ORANGE )
	{
		m_pRenderContext = pRenderContext;
		Assert( m_pRenderContext );
		Assert( szName );
		m_pRenderContext->BeginPIXEvent( color, szName );
	}
	~PIXEvent()
	{
		m_pRenderContext->EndPIXEvent();
	}
private:
	IMatRenderContext *m_pRenderContext;
};


#define PIX_ENABLE 0		// set this to 1 and build engine/studiorender to enable pix events in the engine

#if PIX_ENABLE
#	define PIXEVENT PIXEvent _pixEvent
#else
#	define PIXEVENT 
#endif

//-----------------------------------------------------------------------------

#ifdef MATERIAL_SYSTEM_DEBUG_CALL_QUEUE
#include "tier1/callqueue.h"
#include "tier1/fmtstr.h"
static void DoMatSysQueueMark( IMaterialSystem *pMaterialSystem, const char *psz )
{
	CMatRenderContextPtr pRenderContext( pMaterialSystem );
	if ( pRenderContext->GetCallQueue() ) 
		pRenderContext->GetCallQueue()->QueueCall( Plat_DebugString, CUtlEnvelope<const char *>( psz ) );
}

#define MatSysQueueMark( pMaterialSystem, ...) DoMatSysQueueMark( pMaterialSystem, CFmtStr( __VA_ARGS__ ) )
#else
#define MatSysQueueMark( msg, ...) ((void)0)
#endif

//-----------------------------------------------------------------------------

#ifndef PARTICLES_DLL
DECLARE_TIER2_INTERFACE( IMaterialSystem, materials );
DECLARE_TIER2_INTERFACE( IMaterialSystem, g_pMaterialSystem );
#endif
