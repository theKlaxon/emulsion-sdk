#pragma once
#include "materialsystem_new.h"

extern IMaterialSystemNew* g_pMaterialsNew;

class IMaterialSystem {

public:

	// Placeholder for API revision
	virtual bool Connect(CreateInterfaceFn factory) { return g_pMaterialsNew->Connect(factory); }
	virtual void Disconnect() { g_pMaterialsNew->Disconnect(); }
	virtual void* QueryInterface(const char* pInterfaceName) { return g_pMaterialsNew->QueryInterface(pInterfaceName); }
	virtual InitReturnVal_t Init() { return g_pMaterialsNew->Init(); }
	virtual void Shutdown() { g_pMaterialsNew->Shutdown(); }

	//---------------------------------------------------------
	// Initialization and shutdown
	//---------------------------------------------------------

	// Call this to initialize the material system
	// returns a method to create interfaces in the shader dll
	virtual CreateInterfaceFn	Init(char const* pShaderAPIDLL,
		IMaterialProxyFactory* pMaterialProxyFactory,
		CreateInterfaceFn fileSystemFactory,
		CreateInterfaceFn cvarFactory = NULL) {
		return g_pMaterialsNew->Init(pShaderAPIDLL, pMaterialProxyFactory, fileSystemFactory, cvarFactory);
	}

	// Call this to set an explicit shader version to use 
	// Must be called before Init().
	virtual void				SetShaderAPI(char const* pShaderAPIDLL) { g_pMaterialsNew->SetShaderAPI(pShaderAPIDLL); }

	// Must be called before Init(), if you're going to call it at all...
	virtual void				SetAdapter(int nAdapter, int nFlags) { g_pMaterialsNew->SetAdapter(nAdapter, nFlags); }

	// Call this when the mod has been set up, which may occur after init
	// At this point, the game + gamebin paths have been set up
	virtual void				ModInit() { g_pMaterialsNew->ModInit(); }
	virtual void				ModShutdown() { g_pMaterialsNew->ModShutdown(); }

	//---------------------------------------------------------
	//
	//---------------------------------------------------------
	virtual void				SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) { g_pMaterialsNew->SetThreadMode(mode, nServiceThread); }
	virtual MaterialThreadMode_t GetThreadMode() { return g_pMaterialsNew->GetThreadMode(); }
	virtual void				ExecuteQueued() { g_pMaterialsNew->ExecuteQueued(); }

	//---------------------------------------------------------
	// Config management
	//---------------------------------------------------------

	virtual IMaterialSystemHardwareConfig* GetHardwareConfig(const char* pVersion, int* returnCode) { return g_pMaterialsNew->GetHardwareConfig(pVersion, returnCode); }


	// Call this before rendering each frame with the current config
	// for the material system.
	// Will do whatever is necessary to get the material system into the correct state
	// upon configuration change. .doesn't much else otherwise.
	virtual bool				UpdateConfig(bool bForceUpdate) { return g_pMaterialsNew->UpdateConfig(bForceUpdate); }

	// Force this to be the config; update all material system convars to match the state
	// return true if lightmaps need to be redownloaded
	virtual bool				OverrideConfig(const MaterialSystem_Config_t& config, bool bForceUpdate) { return g_pMaterialsNew->OverrideConfig(config, bForceUpdate); }

	// Get the current config for this video card (as last set by UpdateConfig)
	virtual const MaterialSystem_Config_t& GetCurrentConfigForVideoCard() const { return g_pMaterialsNew->GetCurrentConfigForVideoCard(); }

	// Gets *recommended* configuration information associated with the display card, 
	// given a particular dx level to run under. 
	// Use dxlevel 0 to use the recommended dx level.
	// The function returns false if an invalid dxlevel was specified

	// UNDONE: To find out all convars affected by configuration, we'll need to change
	// the dxsupport.pl program to output all column headers into a single keyvalue block
	// and then we would read that in, and send it back to the client
	virtual bool				GetRecommendedConfigurationInfo(int nDXLevel, KeyValues* pKeyValues) { return g_pMaterialsNew->GetRecommendedConfigurationInfo(nDXLevel, pKeyValues); }


	// -----------------------------------------------------------
	// Device methods
	// -----------------------------------------------------------

	// Gets the number of adapters...
	virtual int					GetDisplayAdapterCount() const { return g_pMaterialsNew->GetDisplayAdapterCount(); }

	// Returns the current adapter in use
	virtual int					GetCurrentAdapter() const { return g_pMaterialsNew->GetCurrentAdapter(); }

	// Returns info about each adapter
	virtual void				GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const { g_pMaterialsNew->GetDisplayAdapterInfo(adapter, info); }

	// Returns the number of modes
	virtual int					GetModeCount(int adapter) const { return g_pMaterialsNew->GetModeCount(adapter); }

	// Returns mode information..
	virtual void				GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const { g_pMaterialsNew->GetModeInfo(adapter, mode, info); }

	virtual void				AddModeChangeCallBack(ModeChangeCallbackFunc_t func) { g_pMaterialsNew->AddModeChangeCallBack(func); }

	// Returns the mode info for the current display device
	virtual void				GetDisplayMode(MaterialVideoMode_t& mode) const { g_pMaterialsNew->GetDisplayMode(mode); }

	// Sets the mode...
	virtual bool				SetMode(void* hwnd, const MaterialSystem_Config_t& config) { return g_pMaterialsNew->SetMode(hwnd, config); }

	virtual bool				SupportsMSAAMode(int nMSAAMode) { return g_pMaterialsNew->SupportsMSAAMode(nMSAAMode); }

	// FIXME: REMOVE! Get video card identitier
	virtual const MaterialSystemHardwareIdentifier_t& GetVideoCardIdentifier(void) const { return g_pMaterialsNew->GetVideoCardIdentifier(); }

	// Use this to spew information about the 3D layer 
	virtual void				SpewDriverInfo() const { g_pMaterialsNew->SpewDriverInfo(); }

	// Get the image format of the back buffer. . useful when creating render targets, etc.
	virtual void				GetBackBufferDimensions(int& width, int& height) const { g_pMaterialsNew->GetBackBufferDimensions(width, height); }
	virtual ImageFormat			GetBackBufferFormat() const { return g_pMaterialsNew->GetBackBufferFormat(); }

	virtual bool				SupportsHDRMode(HDRType_t nHDRModede) { return g_pMaterialsNew->SupportsHDRMode(nHDRModede); }


	// -----------------------------------------------------------
	// Window methods
	// -----------------------------------------------------------

	// Creates/ destroys a child window
	virtual bool				AddView(void* hwnd) { return g_pMaterialsNew->AddView(hwnd); }
	virtual void				RemoveView(void* hwnd) { g_pMaterialsNew->RemoveView(hwnd); }

	// Sets the view
	virtual void				SetView(void* hwnd) { g_pMaterialsNew->SetView(hwnd); }


	// -----------------------------------------------------------
	// Control flow
	// -----------------------------------------------------------

	virtual void				BeginFrame(float frameTime) { g_pMaterialsNew->BeginFrame(frameTime); }
	virtual void				EndFrame() { g_pMaterialsNew->EndFrame(); }
	virtual void				Flush(bool flushHardware = false) { g_pMaterialsNew->Flush(flushHardware); }

	/// FIXME: This stuff needs to be cleaned up and abstracted.
	// Stuff that gets exported to the launcher through the engine
	virtual void				SwapBuffers() { g_pMaterialsNew->SwapBuffers(); }

	// Flushes managed textures from the texture cacher
	virtual void				EvictManagedResources() { g_pMaterialsNew->EvictManagedResources(); }

	virtual void				ReleaseResources(void) { g_pMaterialsNew->ReleaseResources(); }
	virtual void				ReacquireResources(void) { g_pMaterialsNew->ReacquireResources(); }


	// -----------------------------------------------------------
	// Device loss/restore
	// -----------------------------------------------------------

	// Installs a function to be called when we need to release vertex buffers + textures
	virtual void				AddReleaseFunc(MaterialBufferReleaseFunc_t func) { g_pMaterialsNew->AddReleaseFunc(func); }
	virtual void				RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) { g_pMaterialsNew->RemoveReleaseFunc(func); }

	// Installs a function to be called when we need to restore vertex buffers
	virtual void				AddRestoreFunc(MaterialBufferRestoreFunc_t func) { g_pMaterialsNew->AddRestoreFunc(func); }
	virtual void				RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) { g_pMaterialsNew->RemoveRestoreFunc(func); }

	// Installs a function to be called when we need to delete objects at the end of the render frame
	virtual void				AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) { g_pMaterialsNew->AddEndFrameCleanupFunc(func); }
	virtual void				RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) { g_pMaterialsNew->RemoveEndFrameCleanupFunc(func); }

	// Release temporary HW memory...
	virtual void				ResetTempHWMemory(bool bExitingLevel = false) { g_pMaterialsNew->ResetTempHWMemory(bExitingLevel); }

	// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
	virtual void				HandleDeviceLost() { g_pMaterialsNew->HandleDeviceLost(); }


	// -----------------------------------------------------------
	// Shaders
	// -----------------------------------------------------------

	// Used to iterate over all shaders for editing purposes
	// GetShaders returns the number of shaders it actually found
	virtual int					ShaderCount() const { return g_pMaterialsNew->ShaderCount(); }
	virtual int					GetShaders(int nFirstShader, int nMaxCount, IShader** ppShaderList) const { return g_pMaterialsNew->GetShaders(nFirstShader, nMaxCount, ppShaderList); }

	// FIXME: Is there a better way of doing this?
	// Returns shader flag names for editors to be able to edit them
	virtual int					ShaderFlagCount() const { return g_pMaterialsNew->ShaderFlagCount(); }
	virtual const char*			ShaderFlagName(int nIndex) const { return g_pMaterialsNew->ShaderFlagName(nIndex); }

	// Gets the actual shader fallback for a particular shader
	virtual void				GetShaderFallback(const char* pShaderName, char* pFallbackShader, int nFallbackLength) { g_pMaterialsNew->GetShaderFallback(pShaderName, pFallbackShader, nFallbackLength); }


	// -----------------------------------------------------------
	// Material proxies
	// -----------------------------------------------------------

	virtual IMaterialProxyFactory* GetMaterialProxyFactory() { return g_pMaterialsNew->GetMaterialProxyFactory(); }

	// Sets the material proxy factory. Calling this causes all materials to be uncached.
	virtual void				SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) { g_pMaterialsNew->SetMaterialProxyFactory(pFactory); }


	// -----------------------------------------------------------
	// Editor mode
	// -----------------------------------------------------------

	// Used to enable editor materials. Must be called before Init.
	virtual void				EnableEditorMaterials() { g_pMaterialsNew->EnableEditorMaterials(); }
	virtual void                EnableGBuffers() { g_pMaterialsNew->EnableGBuffers(); }

	// -----------------------------------------------------------
	// Stub mode mode
	// -----------------------------------------------------------

	// Force it to ignore Draw calls.
	virtual void				SetInStubMode(bool bInStubMode) { g_pMaterialsNew->SetInStubMode(bInStubMode); }


	//---------------------------------------------------------
	// Debug support
	//---------------------------------------------------------

	virtual void				DebugPrintUsedMaterials(const char* pSearchSubString, bool bVerbose) { g_pMaterialsNew->DebugPrintUsedMaterials(pSearchSubString, bVerbose); }
	virtual void				DebugPrintUsedTextures(void) { g_pMaterialsNew->DebugPrintUsedTextures(); }

	virtual void				ToggleSuppressMaterial(char const* pMaterialName) { g_pMaterialsNew->ToggleSuppressMaterial(pMaterialName); }
	virtual void				ToggleDebugMaterial(char const* pMaterialName) { g_pMaterialsNew->ToggleDebugMaterial(pMaterialName); }


	//---------------------------------------------------------
	// Misc features
	//---------------------------------------------------------
	//returns whether fast clipping is being used or not - needed to be exposed for better per-object clip behavior
	virtual bool				UsingFastClipping(void) { return g_pMaterialsNew->UsingFastClipping(); }

	virtual int					StencilBufferBits(void) { return g_pMaterialsNew->StencilBufferBits(); } //number of bits per pixel in the stencil buffer


	//---------------------------------------------------------
	// Material and texture management
	//---------------------------------------------------------

	// uncache all materials. .  good for forcing reload of materials.
	virtual void				UncacheAllMaterials() { g_pMaterialsNew->UncacheAllMaterials(); }

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void				UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) { g_pMaterialsNew->UncacheUnusedMaterials(bRecomputeStateSnapshots); }

	// Load any materials into memory that are to be used as determined
	// by the IMaterial's reference count.
	virtual void				CacheUsedMaterials() { g_pMaterialsNew->CacheUsedMaterials(); }

	// Force all textures to be reloaded from disk.
	virtual void				ReloadTextures() { g_pMaterialsNew->ReloadTextures(); }

	// Reloads materials
	virtual void				ReloadMaterials(const char* pSubString = NULL) { g_pMaterialsNew->ReloadMaterials(); }

	// Create a procedural material. The keyvalues looks like a VMT file
	virtual IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues) { 
		return g_pMaterialsNew->CreateMaterial(pMaterialName, pVMTKeyValues);
	}

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
	virtual IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = NULL) { return g_pMaterialsNew->FindMaterial(pMaterialName, pTextureGroupName, complain, pComplainPrefix); }

	//---------------------------------
	// This is the interface for knowing what materials are available
	// is to use the following functions to get a list of materials.  The
	// material names will have the full path to the material, and that is the 
	// only way that the directory structure of the materials will be seen through this
	// interface.
	// NOTE:  This is mostly for worldcraft to get a list of materials to put
	// in the "texture" browser.in Worldcraft
	virtual MaterialHandle_t	FirstMaterial() const { return g_pMaterialsNew->FirstMaterial(); }

	// returns InvalidMaterial if there isn't another material.
	// WARNING: you must call GetNextMaterial until it returns NULL, 
	// otherwise there will be a memory leak.
	virtual MaterialHandle_t	NextMaterial(MaterialHandle_t h) const { return g_pMaterialsNew->NextMaterial(h); }

	// This is the invalid material
	virtual MaterialHandle_t	InvalidMaterial() const { return g_pMaterialsNew->InvalidMaterial(); }

	// Returns a particular material
	virtual IMaterial* GetMaterial(MaterialHandle_t h) const { return g_pMaterialsNew->GetMaterial(h); }

	// Get the total number of materials in the system.  These aren't just the used
	// materials, but the complete collection.
	virtual int					GetNumMaterials() const { return g_pMaterialsNew->GetNumMaterials(); }

	//---------------------------------

	virtual ITexture* FindTexture(char const* pTextureName, const char* pTextureGroupName, bool complain = true) { return g_pMaterialsNew->FindTexture(pTextureName, pTextureGroupName, complain); }

	// Checks to see if a particular texture is loaded
	virtual bool				IsTextureLoaded(char const* pTextureName) const { return g_pMaterialsNew->IsTextureLoaded(pTextureName); }

	// Creates a procedural texture
	virtual ITexture* CreateProceduralTexture(const char* pTextureName,
		const char* pTextureGroupName,
		int w,
		int h,
		ImageFormat fmt,
		int nFlags) {
		return g_pMaterialsNew->CreateProceduralTexture(pTextureGroupName, pTextureGroupName, w, h, fmt, nFlags);
	}

	//
	// Render targets
	//
	virtual void				BeginRenderTargetAllocation() { g_pMaterialsNew->BeginRenderTargetAllocation(); }
	virtual void				EndRenderTargetAllocation() {
		g_pMaterialsNew->EndRenderTargetAllocation();
	} // Simulate an Alt-Tab in here, which causes a release/restore of all resources

	// Creates a render target
	// If depth == true, a depth buffer is also allocated. If not, then
	// the screen's depth buffer is used.
	// Creates a texture for use as a render target
	virtual ITexture* CreateRenderTargetTexture(int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat	format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) {
		return g_pMaterialsNew->CreateRenderTargetTexture(w, h, sizeMode, format, depth);
	}

	virtual ITexture* CreateNamedRenderTargetTextureEx(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		unsigned int renderTargetFlags = 0) {
		return g_pMaterialsNew->CreateNamedRenderTargetTextureEx(pRTName, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags);
	}

	virtual ITexture* CreateNamedRenderTargetTexture(const char* pRTName,
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		bool bClampTexCoords = true,
		bool bAutoMipMap = false) {
		return g_pMaterialsNew->CreateNamedRenderTargetTexture(pRTName, w, h, sizeMode, format, depth, bClampTexCoords, bAutoMipMap);
	}

	// Must be called between the above Begin-End calls!
	virtual ITexture* CreateNamedRenderTargetTextureEx2(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		RenderTargetSizeMode_t sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat format,
		MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED,
		unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
		unsigned int renderTargetFlags = 0) {
		return g_pMaterialsNew->CreateNamedRenderTargetTextureEx2(pRTName, w, h, sizeMode, format, depth, textureFlags, renderTargetFlags);
	}

	// -----------------------------------------------------------
	// Lightmaps
	// -----------------------------------------------------------

	virtual void				BeginLightmapAllocation() { g_pMaterialsNew->BeginLightmapAllocation(); }
	virtual void				EndLightmapAllocation() { g_pMaterialsNew->EndLightmapAllocation(); }

	virtual int 				AllocateLightmap(int width, int height, int offsetIntoLightmapPage[2], IMaterial* pMaterial) {
		return g_pMaterialsNew->AllocateLightmap(width, height, offsetIntoLightmapPage, pMaterial);
	}

	virtual int					AllocateWhiteLightmap(IMaterial* pMaterial) { return g_pMaterialsNew->AllocateWhiteLightmap(pMaterial); }

	virtual void				UpdateLightmap(int lightmapPageID, int lightmapSize[2],
		int offsetIntoLightmapPage[2],
		float* pFloatImage, float* pFloatImageBump1,
		float* pFloatImageBump2, float* pFloatImageBump3) {
		g_pMaterialsNew->UpdateLightmap(lightmapPageID, lightmapSize, offsetIntoLightmapPage, pFloatImage, pFloatImageBump1, pFloatImageBump2, pFloatImageBump3);
	}

	// fixme: could just be an array of ints for lightmapPageIDs since the material
	// for a surface is already known.
	virtual int					GetNumSortIDs() { return g_pMaterialsNew->GetNumSortIDs(); }
	virtual void				GetSortInfo(MaterialSystem_SortInfo_t* sortInfoArray) { g_pMaterialsNew->GetSortInfo(sortInfoArray); }

	// Read the page size of an existing lightmap by sort id (returned from AllocateLightmap())
	virtual void				GetLightmapPageSize(int lightmap, int* width, int* height) const { return g_pMaterialsNew->GetLightmapPageSize(lightmap, width, height); }
	virtual void				ResetMaterialLightmapPageInfo() { g_pMaterialsNew->ResetMaterialLightmapPageInfo(); }
	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) { g_pMaterialsNew->ClearBuffers(bClearColor, bClearDepth, bClearStencil); }

	// -----------------------------------------------------------
	// Access the render contexts
	// -----------------------------------------------------------
	virtual IMatRenderContext* GetRenderContext() { return g_pMaterialsNew->GetRenderContext(); }

	virtual void				BeginUpdateLightmaps(void) { g_pMaterialsNew->BeginUpdateLightmaps(); }
	virtual void				EndUpdateLightmaps(void) { g_pMaterialsNew->EndUpdateLightmaps(); }

	// -----------------------------------------------------------
	// Methods to force the material system into non-threaded, non-queued mode
	// -----------------------------------------------------------
	virtual MaterialLock_t		Lock() { return g_pMaterialsNew->Lock(); }
	virtual void				Unlock(MaterialLock_t p1) { g_pMaterialsNew->Unlock(p1); }

	// Create a custom render context. Cannot be used to create MATERIAL_HARDWARE_CONTEXT
	virtual IMatRenderContext* CreateRenderContext(MaterialContextType_t type) { return g_pMaterialsNew->CreateRenderContext(type); }

	// Set a specified render context to be the global context for the thread. Returns the prior context.
	virtual IMatRenderContext* SetRenderContext(IMatRenderContext* pRenderContext) { return g_pMaterialsNew->SetRenderContext(pRenderContext); }

	virtual bool				SupportsCSAAMode(int nNumSamples, int nQualityLevel) { return g_pMaterialsNew->SupportsCSAAMode(nNumSamples, nQualityLevel); }

	virtual void				RemoveModeChangeCallBack(ModeChangeCallbackFunc_t func) { g_pMaterialsNew->RemoveModeChangeCallBack(func); }

	// Finds or create a procedural material.
	virtual IMaterial* FindProceduralMaterial(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues) { return g_pMaterialsNew->FindProceduralMaterial(pMaterialName, pTextureGroupName, pVMTKeyValues); }

	virtual void				AddTextureAlias(const char* pAlias, const char* pRealName) { g_pMaterialsNew->AddTextureAlias(pAlias, pRealName); }
	virtual void				RemoveTextureAlias(const char* pAlias) { g_pMaterialsNew->RemoveTextureAlias(pAlias); }

	// returns a lightmap page ID for this allocation, -1 if none available
	// frameID is a number that should be changed every frame to prevent locking any textures that are
	// being used to draw in the previous frame
	virtual int					AllocateDynamicLightmap(int lightmapSize[2], int* pOutOffsetIntoPage, int frameID) { return g_pMaterialsNew->AllocateDynamicLightmap(lightmapSize, pOutOffsetIntoPage, frameID); }

	virtual void				SetExcludedTextures(const char* pScriptName) { g_pMaterialsNew->SetExcludedTextures(pScriptName); }
	virtual void				UpdateExcludedTextures(void) { g_pMaterialsNew->UpdateExcludedTextures(); }

	virtual bool				IsInFrame() const { return g_pMaterialsNew->IsInFrame(); }

	virtual void				CompactMemory() { g_pMaterialsNew->CompactMemory(); }

	// For sv_pure mode. The filesystem figures out which files the client needs to reload to be "pure" ala the server's preferences.
	virtual void ReloadFilesInList(IFileList* pFilesToReload) { g_pMaterialsNew->ReloadFilesInList(pFilesToReload); }

	// Get information about the texture for texture management tools
	virtual bool				GetTextureInformation(char const* szTextureName, MaterialTextureInfo_t& info) const { return g_pMaterialsNew->GetTextureInformation(szTextureName, info); }

	// call this once the render targets are allocated permanently at the beginning of the game
	virtual void FinishRenderTargetAllocation(void) { g_pMaterialsNew->FinishRenderTargetAllocation(); }

	virtual void ReEnableRenderTargetAllocation_IRealizeIfICallThisAllTexturesWillBeUnloadedAndLoadTimeWillSufferHorribly(void) { g_pMaterialsNew->ReEnableRenderTargetAllocation_IRealizeIfICallThisAllTexturesWillBeUnloadedAndLoadTimeWillSufferHorribly(); }
	virtual	bool				AllowThreading(bool bAllow, int nServiceThread) { return g_pMaterialsNew->AllowThreading(bAllow, nServiceThread); }

	virtual bool				GetRecommendedVideoConfig(KeyValues* pKeyValues) { return g_pMaterialsNew->GetRecommendedVideoConfig(pKeyValues); }

	virtual IClientMaterialSystem* GetClientMaterialSystemInterface() { return g_pMaterialsNew->GetClientMaterialSystemInterface(); }

	virtual bool				CanDownloadTextures() const { return g_pMaterialsNew->CanDownloadTextures(); }
	virtual int					GetNumLightmapPages() const { return g_pMaterialsNew->GetNumLightmapPages(); }

	virtual IPaintMapTextureManager* RegisterPaintMapDataManager(IPaintMapDataManager* pDataManager) { return g_pMaterialsNew->RegisterPaintMapDataManager(pDataManager); } //You supply an interface we can query for bits, it gives back an interface you can use to drive updates

};

class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> BaseClass;
public:
	CMatRenderContextPtr() {}
	CMatRenderContextPtr(IMatRenderContext* pInit) : BaseClass(pInit) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	CMatRenderContextPtr(IMaterialSystem* pFrom) : BaseClass(pFrom->GetRenderContext()) { if (BaseClass::m_pObject) BaseClass::m_pObject->BeginRender(); }
	~CMatRenderContextPtr() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); }

	IMatRenderContext* operator=(IMatRenderContext* p) { if (p) p->BeginRender(); return BaseClass::operator=(p); }

	void SafeRelease() { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::SafeRelease(); }
	void AssignAddRef(IMatRenderContext* pFrom) { if (BaseClass::m_pObject) BaseClass::m_pObject->EndRender(); BaseClass::AssignAddRef(pFrom); BaseClass::m_pObject->BeginRender(); }

	void GetFrom(IMaterialSystem* pFrom) { AssignAddRef(pFrom->GetRenderContext()); }


private:
	CMatRenderContextPtr(const CMatRenderContextPtr& from);
	void operator=(const CMatRenderContextPtr& from);

};