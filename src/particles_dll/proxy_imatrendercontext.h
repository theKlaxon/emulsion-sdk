#pragma once
#include "materialsystem/imaterialsystem.h"
#include "tier1/refcount.h"

class IMatRenderContASW {
public:
	//IRefCounted Interface
	virtual int					AddRef() {return m_pRenderContext->AddRef();}
	virtual int					Release() {return m_pRenderContext->Release();}


	virtual void				BeginRender() { m_pRenderContext->BeginRender(); }
	virtual void				EndRender() { m_pRenderContext->EndRender(); }

	virtual void				Flush(bool flushHardware = false) { m_pRenderContext->Flush(); }

	virtual void				BindLocalCubemap(ITexture* pTexture) { m_pRenderContext->BindLocalCubemap(pTexture); }
	
	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture* pTexture) { m_pRenderContext->SetRenderTarget(pTexture); }
	virtual ITexture*			GetRenderTarget(void) { return m_pRenderContext->GetRenderTarget(); }

	virtual void				GetRenderTargetDimensions(int& width, int& height) const { m_pRenderContext->GetRenderTargetDimensions(width, height); }

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial* material, void* proxyData = 0) { m_pRenderContext->Bind(material, proxyData); }
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) { m_pRenderContext->BindLightmapPage(lightmapPageID); }

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) { m_pRenderContext->DepthRange(zNear, zFar); }

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) { m_pRenderContext->ClearBuffers(bClearColor, bClearDepth, bClearStencil); }

	// read to a unsigned char rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, unsigned char* data, ImageFormat dstFormat) { m_pRenderContext->ReadPixels(x, y, width, height, data, dstFormat); }

	// Sets lighting
	virtual void				SetLightingState(const MaterialLightingState_t& state) { m_pRenderContext->SetLightingState(state); }
	virtual void				SetLights(int nCount, const LightDesc_t* pLights) { m_pRenderContext->SetLights(nCount, pLights); }

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D cube[6]) { m_pRenderContext->SetAmbientLightCube(cube); }

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture* pTexture) { m_pRenderContext->CopyRenderTargetToTexture(pTexture); }

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) { m_pRenderContext->SetFrameBufferCopyTexture(pTexture, textureIndex); }
	virtual ITexture*			GetFrameBufferCopyTexture(int textureIndex) { return m_pRenderContext->GetFrameBufferCopyTexture(textureIndex); }

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(MaterialMatrixMode_t matrixMode) { m_pRenderContext->MatrixMode(matrixMode); }
	virtual void				PushMatrix(void) { m_pRenderContext->PushMatrix(); }
	virtual void				PopMatrix(void) { m_pRenderContext->PopMatrix(); }
	virtual void				LoadMatrix(VMatrix const& matrix) { m_pRenderContext->LoadMatrix(matrix); }
	virtual void				LoadMatrix(matrix3x4_t const& matrix) { m_pRenderContext->LoadMatrix(matrix); }
	virtual void				MultMatrix(VMatrix const& matrix) { m_pRenderContext->MultMatrix(matrix); }
	virtual void				MultMatrix(matrix3x4_t const& matrix) { m_pRenderContext->MultMatrix(matrix); }
	virtual void				MultMatrixLocal(VMatrix const& matrix) { m_pRenderContext->MultMatrixLocal(matrix); }
	virtual void				MultMatrixLocal(matrix3x4_t const& matrix) { m_pRenderContext->MultMatrixLocal(matrix); }
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix* matrix) { m_pRenderContext->GetMatrix(matrixMode, matrix); }
	virtual void				GetMatrix(MaterialMatrixMode_t matrixMode, matrix3x4_t* matrix) { m_pRenderContext->GetMatrix(matrixMode, matrix); }
	virtual void				LoadIdentity(void) { m_pRenderContext->LoadIdentity(); }
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) { m_pRenderContext->Ortho(left, top, right, bottom, zNear, zFar); }
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) { m_pRenderContext->PerspectiveX(fovx, aspect, zNear, zFar); }
	virtual void				PickMatrix(int x, int y, int width, int height) { m_pRenderContext->PickMatrix(x, y, width, height); }
	virtual void				Rotate(float angle, float x, float y, float z) { m_pRenderContext->Rotate(angle, x, y, z); }
	virtual void				Translate(float x, float y, float z) { m_pRenderContext->Translate(x, y, z); }
	virtual void				Scale(float x, float y, float z) { m_pRenderContext->Scale(x, y, z); }
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) { m_pRenderContext->Viewport(x, y, width, height); }
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const { m_pRenderContext->GetViewport(x, y, width, height); }

	// The cull mode
	virtual void				CullMode(MaterialCullMode_t cullMode) { m_pRenderContext->CullMode(cullMode); }
	virtual void				FlipCullMode(void) {
		m_pRenderContext->FlipCullMode();
	} //CW->CCW or CCW->CW, intended for mirror support where the view matrix is flipped horizontally

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(MaterialHeightClipMode_t nHeightClipMode) { m_pRenderContext->SetHeightClipMode(nHeightClipMode); }
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) { m_pRenderContext->SetHeightClipZ(z); }

	// Fog methods...
	virtual void				FogMode(MaterialFogMode_t fogMode) { m_pRenderContext->FogMode(fogMode); }
	virtual void				FogStart(float fStart) { m_pRenderContext->FogStart(fStart); }
	virtual void				FogEnd(float fEnd) { m_pRenderContext->FogEnd(fEnd); }
	virtual void				SetFogZ(float fogZ) { m_pRenderContext->SetFogZ(fogZ); }
	virtual MaterialFogMode_t	GetFogMode(void) { return m_pRenderContext->GetFogMode(); }

	virtual void				FogColor3f(float r, float g, float b) { m_pRenderContext->FogColor3f(r, g, b); }
	virtual void				FogColor3fv(float const* rgb) { m_pRenderContext->FogColor3fv(rgb); }
	virtual void				FogColor3ub(unsigned char r, unsigned char g, unsigned char b) { m_pRenderContext->FogColor3ub(r, g, b); }
	virtual void				FogColor3ubv(unsigned char const* rgb) { m_pRenderContext->FogColor3ubv(rgb); }

	virtual void				GetFogColor(unsigned char* rgb) { m_pRenderContext->GetFogColor(rgb); }

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) { m_pRenderContext->SetNumBoneWeights(numBones); }

	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t fmt, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL, VertexStreamSpec_t* pStreamSpec = NULL) { return m_pRenderContext->CreateStaticMesh(fmt, pTextureBudgetGroup, pMaterial, pStreamSpec); }
	virtual void DestroyStaticMesh(IMesh* mesh) { m_pRenderContext->DestroyStaticMesh(mesh); }
	
	virtual IMesh* GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial* pAutoBind = 0) {
		return m_pRenderContext->GetDynamicMesh(buffered, pVertexOverride, pIndexOverride, pAutoBind);
	}

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer* CreateStaticVertexBuffer(VertexFormat_t fmt, int nVertexCount, const char* pTextureBudgetGroup) { return m_pRenderContext->CreateStaticVertexBuffer(fmt, nVertexCount, pTextureBudgetGroup); }
	virtual IIndexBuffer* CreateStaticIndexBuffer(MaterialIndexFormat_t fmt, int nIndexCount, const char* pTextureBudgetGroup) { return m_pRenderContext->CreateStaticIndexBuffer(fmt, nIndexCount, pTextureBudgetGroup); }
	virtual void DestroyVertexBuffer(IVertexBuffer* b) { m_pRenderContext->DestroyVertexBuffer(b); }
	virtual void DestroyIndexBuffer(IIndexBuffer* b) { m_pRenderContext->DestroyIndexBuffer(b); }
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer* GetDynamicVertexBuffer(int streamID, VertexFormat_t vertexFormat, bool bBuffered = true) { return m_pRenderContext->GetDynamicVertexBuffer(streamID, vertexFormat, bBuffered); }
	virtual IIndexBuffer* GetDynamicIndexBuffer() { return m_pRenderContext->GetDynamicIndexBuffer(); }
	virtual void BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, VertexFormat_t fmt, int nRepetitions = 1) { m_pRenderContext->BindVertexBuffer(streamID, pVertexBuffer, nOffsetInBytes, nFirstVertex, nVertexCount, fmt, nRepetitions); }
	virtual void BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) { m_pRenderContext->BindIndexBuffer(pIndexBuffer, nOffsetInBytes); }
	virtual void Draw(MaterialPrimitiveType_t primitiveType, int firstIndex, int numIndices) { m_pRenderContext->Draw(primitiveType, firstIndex, numIndices); }
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int  SelectionMode(bool selectionMode) { return m_pRenderContext->SelectionMode(selectionMode); }
	virtual void SelectionBuffer(unsigned int* pBuffer, int size) { m_pRenderContext->SelectionBuffer(pBuffer, size); }
	virtual void ClearSelectionNames() { m_pRenderContext->ClearSelectionNames(); }
	virtual void LoadSelectionName(int name) { m_pRenderContext->LoadSelectionName(name); }
	virtual void PushSelectionName(int name) { m_pRenderContext->PushSelectionName(name); }
	virtual void PopSelectionName() { m_pRenderContext->PopSelectionName(); }

	// Sets the Clear Color for ClearBuffer....
	virtual void		ClearColor3ub(unsigned char r, unsigned char g, unsigned char b) { m_pRenderContext->ClearColor3ub(r, g, b); }
	virtual void		ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { m_pRenderContext->ClearColor4ub(r, g, b, a); }

	// Allows us to override the depth buffer setting of a material
	virtual void	OverrideDepthEnable(bool bEnable, bool bDepthEnable) { m_pRenderContext->OverrideDepthEnable(bEnable, bDepthEnable); }

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void	DrawScreenSpaceQuad(IMaterial* pMaterial) { m_pRenderContext->DrawScreenSpaceQuad(pMaterial); }

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void	SyncToken(const char* pToken) { m_pRenderContext->SyncToken(pToken); }

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float	ComputePixelWidthOfSphere(const Vector& origin, float flRadius) { return m_pRenderContext->ComputePixelWidthOfSphere(origin, flRadius); }

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual OcclusionQueryObjectHandle_t CreateOcclusionQueryObject(void) { return m_pRenderContext->CreateOcclusionQueryObject(); }
	virtual void DestroyOcclusionQueryObject(OcclusionQueryObjectHandle_t h) { m_pRenderContext->DestroyOcclusionQueryObject(h); }

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void BeginOcclusionQueryDrawing(OcclusionQueryObjectHandle_t h) { m_pRenderContext->BeginOcclusionQueryDrawing(h); }
	virtual void EndOcclusionQueryDrawing(OcclusionQueryObjectHandle_t h) { m_pRenderContext->EndOcclusionQueryDrawing(h); }

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int OcclusionQuery_GetNumPixelsRendered(OcclusionQueryObjectHandle_t h) { return m_pRenderContext->OcclusionQuery_GetNumPixelsRendered(h); }

	virtual void SetFlashlightMode(bool bEnable) { m_pRenderContext->SetFlashlightMode(bEnable); }

	virtual void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) { m_pRenderContext->SetFlashlightState(state, worldToTexture); }

	// Gets the current height clip mode
	virtual MaterialHeightClipMode_t GetHeightClipMode() { return m_pRenderContext->GetHeightClipMode(); }

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float	ComputePixelDiameterOfSphere(const Vector& vecAbsOrigin, float flRadius) { return m_pRenderContext->ComputePixelDiameterOfSphere(vecAbsOrigin, flRadius); }

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void	EnableUserClipTransformOverride(bool bEnable) { m_pRenderContext->EnableUserClipTransformOverride(bEnable); }
	virtual void	UserClipTransform(const VMatrix& worldToView) { m_pRenderContext->UserClipTransform(worldToView); }

	virtual bool GetFlashlightMode() const { return m_pRenderContext->GetFlashlightMode(); }

	// Used to make the handle think it's never had a successful query before
	virtual void ResetOcclusionQueryObject(OcclusionQueryObjectHandle_t h) { m_pRenderContext->ResetOcclusionQueryObject(h); }

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph* CreateMorph(MorphFormat_t format, const char* pDebugName) { return m_pRenderContext->CreateMorph(format, pDebugName); }
	virtual void DestroyMorph(IMorph* pMorph) { m_pRenderContext->DestroyMorph(pMorph); }

	// Binds the morph data for use in rendering
	virtual void BindMorph(IMorph* pMorph) { m_pRenderContext->BindMorph(pMorph); }

	// Sets flexweights for rendering
	virtual void SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) { m_pRenderContext->SetFlexWeights(nFirstWeight, nCount, pWeights); }

	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void* LockRenderData(int nSizeInBytes) { return m_pRenderContext->LockRenderData(nSizeInBytes); }
	virtual void UnlockRenderData(void* pData) { m_pRenderContext->UnlockRenderData(pData); }

	// Typed version. If specified, pSrcData is copied into the locked memory.
	template< class E > E* LockRenderDataTyped(int nCount, const E* pSrcData = NULL) { return m_pRenderContext->LockRenderDataTyped<E>(nCount, pSrcData); }

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void			AddRefRenderData() { m_pRenderContext->AddRefRenderData(); }
	virtual void			ReleaseRenderData() { m_pRenderContext->ReleaseRenderData(); }

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool			IsRenderData(const void* pData) const { return m_pRenderContext->IsRenderData(pData); }

	// Read w/ stretch to a host-memory buffer
	virtual void ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, unsigned char* pBuffer, ImageFormat dstFormat, int nDstStride) { m_pRenderContext->ReadPixelsAndStretch(pSrcRect, pDstRect, pBuffer, dstFormat, nDstStride); }

	// Gets the window size
	virtual void GetWindowSize(int& width, int& height) const { m_pRenderContext->GetWindowSize(width, height); }

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
		m_pRenderContext->DrawScreenSpaceRectangle(pMaterial, destx, desty, width, height, src_texture_x0, src_texture_y0, src_texture_x1, src_texture_y1, src_texture_width, src_texture_height, pClientRenderable, nXDice, nYDice);
	}

	virtual void LoadBoneMatrix(int boneIndex, const matrix3x4_t& matrix) { m_pRenderContext->LoadBoneMatrix(boneIndex, matrix); }

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport() { m_pRenderContext->PushRenderTargetAndViewport(); }

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture) { m_pRenderContext->PushRenderTargetAndViewport(pTexture); }

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) { m_pRenderContext->PushRenderTargetAndViewport(pTexture, nViewX, nViewY, nViewX, nViewH); }

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) { m_pRenderContext->PushRenderTargetAndViewport(pTexture, pDepthTexture, nViewX, nViewY, nViewX, nViewH); }

	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport(void) { m_pRenderContext->PopRenderTargetAndViewport(); }

	// Binds a particular texture as the current lightmap
	virtual void BindLightmapTexture(ITexture* pLightmapTexture) { m_pRenderContext->BindLightmapTexture(pLightmapTexture); }

	// Blit a subrect of the current render target to another texture
	virtual void CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = NULL) { m_pRenderContext->CopyRenderTargetToTextureEx(pTexture, nRenderTargetID, pSrcRect, pDstRect); }

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) { m_pRenderContext->PerspectiveOffCenterX(fovx, aspect, zNear, zFar, bottom, top, left, right); }

	// Sets the ambient light color
	virtual void SetAmbientLightColor(float r, float g, float b) { m_pRenderContext->SetAmbientLightColor(r, g, b); }

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void SetFloatRenderingParameter(int parm_number, float value) { m_pRenderContext->SetFloatRenderingParameter(parm_number, value); }
	virtual void SetIntRenderingParameter(int parm_number, int value) { m_pRenderContext->SetIntRenderingParameter(parm_number, value); }
	virtual void SetVectorRenderingParameter(int parm_number, Vector const& value) { m_pRenderContext->SetVectorRenderingParameter(parm_number, value); }

	// stencil buffer operations.
	virtual void SetStencilState(const ShaderStencilState_t& state) { m_pRenderContext->SetStencilState(state); }
	virtual void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) { m_pRenderContext->ClearStencilBufferRectangle(xmin, ymin, xmax, ymax, value); }

	virtual void SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) { m_pRenderContext->SetRenderTargetEx(nRenderTargetID, pTexture); }

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void PushCustomClipPlane(const float* pPlane) { m_pRenderContext->PushCustomClipPlane(pPlane); }
	virtual void PopCustomClipPlane(void) { m_pRenderContext->PopCustomClipPlane(); }

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) { m_pRenderContext->GetMaxToRender(pMesh, bMaxUntilFlush, pMaxVerts, pMaxIndices); }

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int GetMaxVerticesToRender(IMaterial* pMaterial) { return m_pRenderContext->GetMaxVerticesToRender(pMaterial); } // 1f0?
	virtual int GetMaxIndicesToRender() { return m_pRenderContext->GetMaxIndicesToRender(); }
	virtual void DisableAllLocalLights() { m_pRenderContext->DisableAllLocalLights(); }
	virtual int CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) { return m_pRenderContext->CompareMaterialCombos(pMaterial1, pMaterial2, lightMapID1, lightMapID2); }

	virtual IMesh* GetFlexMesh() { return m_pRenderContext->GetFlexMesh(); }

	virtual void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture, ITexture* pFlashlightDepthTexture) { m_pRenderContext->SetFlashlightStateEx(state, worldToTexture, pFlashlightDepthTexture); }
	virtual void SetFlashlightStateExDeRef(const FlashlightState_t& state, ITexture* pFlashlightDepthTexture) { } // not present in portal 2

	// Returns the currently bound local cubemap
	virtual ITexture* GetLocalCubemap() { return m_pRenderContext->GetLocalCubemap(); }

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) { m_pRenderContext->ClearBuffersObeyStencil(bClearColor, bClearDepth); }

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool EnableClipping(bool bEnable) { return m_pRenderContext->EnableClipping(bEnable); }

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void GetFogDistances(float* fStart, float* fEnd, float* fFogZ) { m_pRenderContext->GetFogDistances(fStart, fEnd, fFogZ); }

	// Hooks for firing PIX events from outside the Material System...
	virtual void BeginPIXEvent(unsigned long color, const char* szName) { m_pRenderContext->BeginPIXEvent(color, szName); }
	virtual void EndPIXEvent() { m_pRenderContext->EndPIXEvent(); }
	virtual void SetPIXMarker(unsigned long color, const char* szName) { m_pRenderContext->SetPIXMarker(color, szName); }

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void BeginBatch(IMesh* pIndices) { m_pRenderContext->BeginBatch(pIndices); }
	virtual void BindBatch(IMesh* pVertices, IMaterial* pAutoBind = NULL) { m_pRenderContext->BindBatch(pVertices, pAutoBind); }
	virtual void DrawBatch(int firstIndex, int numIndices) { m_pRenderContext->DrawBatch(firstIndex, numIndices); }
	virtual void EndBatch() { m_pRenderContext->EndBatch(); }

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue* GetCallQueue() { return m_pRenderContext->GetCallQueue(); }

	// Returns the world-space camera position
	virtual void GetWorldSpaceCameraPosition(Vector* pCameraPos) { 
		m_pRenderContext->GetWorldSpaceCameraPosition(pCameraPos);
	}
	virtual void GetWorldSpaceCameraVectors(Vector* pVecForward, Vector* pVecRight, Vector* pVecUp) { m_pRenderContext->GetWorldSpaceCameraVectors(pVecForward, pVecRight, pVecUp); }

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector& scale) { m_pRenderContext->SetToneMappingScaleLinear(scale); }
	virtual Vector				GetToneMappingScaleLinear(void) { return m_pRenderContext->GetToneMappingScaleLinear(); }

	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) { m_pRenderContext->SetShadowDepthBiasFactors(fSlopeScaleDepthBias, fDepthBias); }

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation(void) { m_pRenderContext->PerformFullScreenStencilOperation(); }

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector vLightingOrigin) { m_pRenderContext->SetLightingOrigin(vLightingOrigin); }

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) { m_pRenderContext->PushScissorRect(nLeft, nTop, nRight, nBottom, bEnableScissor); }

	// Methods used to build the morph accumulator that is read from when HW morphing is enabled.
	virtual void				BeginMorphAccumulation() { m_pRenderContext->BeginMorphAccumulation(); }
	virtual void				EndMorphAccumulation() { m_pRenderContext->EndMorphAccumulation(); }
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) { m_pRenderContext->AccumulateMorph(pMorph, nMorphCount, pWeights); }

	virtual void				PushDeformation(DeformationBase_t const* Deformation) { m_pRenderContext->PushDeformation(Deformation); }
	virtual void				PopDeformation() { m_pRenderContext->PopDeformation(); }
	virtual int					GetNumActiveDeformations() const { return m_pRenderContext->GetNumActiveDeformations(); }

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D* pTexCoord, IMorph* pMorph, int nVertex) { return m_pRenderContext->GetMorphAccumulatorTexCoord(pTexCoord, pMorph, nVertex); }

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh* GetDynamicMeshEx(VertexFormat_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0, IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) {
		return m_pRenderContext->GetDynamicMeshEx(vertexFormat, bBuffered, pVertexOverride, pIndexOverride, pAutoBind);
	}

	virtual void				FogMaxDensity(float flMaxDensity) { m_pRenderContext->FogMaxDensity(flMaxDensity); }

	virtual IMaterial* GetCurrentMaterial() { return m_pRenderContext->GetCurrentMaterial(); }
	virtual int  GetCurrentNumBones() const { return m_pRenderContext->GetCurrentNumBones(); }
	virtual void* GetCurrentProxy() { return m_pRenderContext->GetCurrentProxy(); }

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void EnableColorCorrection(bool bEnable) { m_pRenderContext->EnableColorCorrection(bEnable); }
	virtual ColorCorrectionHandle_t AddLookup(const char* pName) { return m_pRenderContext->AddLookup(pName); }
	virtual bool RemoveLookup(ColorCorrectionHandle_t handle) { return m_pRenderContext->RemoveLookup(handle); }
	virtual void LockLookup(ColorCorrectionHandle_t handle) { m_pRenderContext->LockLookup(handle); }
	virtual void LoadLookup(ColorCorrectionHandle_t handle, const char* pLookupName) { m_pRenderContext->LoadLookup(handle, pLookupName); }
	virtual void UnlockLookup(ColorCorrectionHandle_t handle) { m_pRenderContext->UnlockLookup(handle); }
	virtual void SetLookupWeight(ColorCorrectionHandle_t handle, float flWeight) { m_pRenderContext->SetLookupWeight(handle, flWeight); }
	virtual void ResetLookupWeights() { m_pRenderContext->ResetLookupWeights(); }
	virtual void SetResetable(ColorCorrectionHandle_t handle, bool bResetable) { m_pRenderContext->SetResetable(handle, bResetable); }

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void SetFullScreenDepthTextureValidityFlag(bool bIsValid) { m_pRenderContext->SetFullScreenDepthTextureValidityFlag(bIsValid); }

	// A special path used to tick the front buffer while loading on the 360
	virtual void SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) { m_pRenderContext->SetNonInteractivePacifierTexture(pTexture, flNormalizedX, flNormalizedY, flNormalizedSize); }
	virtual void SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, MaterialNonInteractiveMode_t mode) { m_pRenderContext->SetNonInteractiveTempFullscreenBuffer(pTexture, mode); }
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode) { m_pRenderContext->EnableNonInteractiveMode(mode); }
	virtual void RefreshFrontBufferNonInteractive() { m_pRenderContext->RefreshFrontBufferNonInteractive(); }

	// Flip culling state (swap CCW <-> CW)
	virtual void FlipCulling(bool bFlipCulling) { m_pRenderContext->FlipCulling(bFlipCulling); }

	virtual void SetTextureRenderingParameter(int parm_number, ITexture* pTexture) { m_pRenderContext->SetTextureRenderingParameter(parm_number, pTexture); }

	//only actually sets a bool that can be read in from shaders, doesn't do any of the legwork
	virtual void EnableSinglePassFlashlightMode(bool bEnable) { m_pRenderContext->EnableSinglePassFlashlightMode(bEnable); }

	// Draws instances with different meshes
	virtual void DrawInstances(int nInstanceCount, const MeshInstanceData_t* pInstance) { m_pRenderContext->DrawInstances(nInstanceCount, pInstance); }

	// Allows us to override the color/alpha write settings of a material
	virtual void OverrideAlphaWriteEnable(bool bOverrideEnable, bool bAlphaWriteEnable) { m_pRenderContext->OverrideAlphaWriteEnable(bOverrideEnable, bAlphaWriteEnable); }
	virtual void OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) { m_pRenderContext->OverrideColorWriteEnable(bOverrideEnable, bColorWriteEnable); }

	virtual void ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) { m_pRenderContext->ClearBuffersObeyStencilEx(bClearColor, bClearAlpha, bClearDepth); }

	// Subdivision surface interface
	virtual int GetSubDBufferWidth() { return m_pRenderContext->GetSubDBufferWidth(); }
	virtual float* LockSubDBuffer(int nNumRows) { return m_pRenderContext->LockSubDBuffer(nNumRows); }
	virtual void UnlockSubDBuffer() { return m_pRenderContext->UnlockSubDBuffer(); }

	// Update current frame's game time for the shader api.
	virtual void UpdateGameTime(float flTime) { m_pRenderContext->UpdateGameTime(flTime); }

	virtual void			PrintfVA(char* fmt, va_list vargs) { m_pRenderContext->PrintfVA(fmt, vargs); }
	virtual void			Printf(char* fmt, ...) {  } // idk how to do this one so im not gonna
	virtual float			Knob(char* knobname, float* setvalue = NULL) { return m_pRenderContext->Knob(knobname, setvalue); }

	// this is here to manage the creation of render contexts
	void SetProtectedContext(IMatRenderContext* pContext) {
		m_pRenderContext = pContext;
	}

protected:

	IMatRenderContext* m_pRenderContext;

};

static IMatRenderContASW pContASW;
//
//void CBeamSegDraw::Start(IMatRenderContext* pRenderContext, int nSegs, IMaterial* pMaterial, CMeshBuilder* pMeshBuilder, int nMeshVertCount) {
//	
//	pContASW.SetProtectedContext(pRenderContext);
//	Start(&pContASW, nSegs, pMaterial, pMeshBuilder, nMeshVertCount);
//}