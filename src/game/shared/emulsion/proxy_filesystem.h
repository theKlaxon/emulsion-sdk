#pragma once
#include "filesystem.h"
#ifdef CLIENT_DLL
#include "cdll_client_int.h"
#else
extern IFileSystem* filesystem;
#endif

//-----------------------------------------------------------------------------
// ASW / Proxy - Main file system interface
//-----------------------------------------------------------------------------
abstract_class IFileSysPrx : public IAppSystem, public IBaseFileSystem
{
public: // appsystem overrides

	virtual bool Connect(CreateInterfaceFn factory) { return filesystem->Connect(factory); }
	virtual void Disconnect() { filesystem->Disconnect(); }

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void* QueryInterface(const char* pInterfaceName) { return filesystem->QueryInterface(pInterfaceName); }

	// Init, shutdown
	virtual InitReturnVal_t Init() { return filesystem->Init(); }
	virtual void Shutdown() { filesystem->Shutdown(); }

	// Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() { return filesystem->GetDependencies(); }

	// Returns the tier
	virtual AppSystemTier_t GetTier() { return filesystem->GetTier(); }

	// Reconnect to a particular interface
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) { filesystem->Reconnect(factory, pInterfaceName); }

public: // IBaseFileSystem overrides

	virtual int				Read(void* pOutput, int size, FileHandle_t file) { return filesystem->Read(pOutput, size, file); }
	virtual int				Write(void const* pInput, int size, FileHandle_t file) { return filesystem->Write(pInput, size, file); }

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open(const char* pFileName, const char* pOptions, const char* pathID = 0) { return filesystem->Open(pFileName, pOptions, pathID); }
	virtual void			Close(FileHandle_t file) { filesystem->Close(file); }


	virtual void			Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType) { filesystem->Seek(file, pos, seekType); }
	virtual unsigned int	Tell(FileHandle_t file) { return filesystem->Tell(file); }
	virtual unsigned int	Size(FileHandle_t file) { return filesystem->Size(file); }
	virtual unsigned int	Size(const char* pFileName, const char* pPathID = 0) { return filesystem->Size(pFileName, pPathID); }

	virtual void			Flush(FileHandle_t file) { filesystem->Flush(file); }
	virtual bool			Precache(const char* pFileName, const char* pPathID = 0) { return filesystem->Precache(pFileName, pPathID); }

	virtual bool			FileExists(const char* pFileName, const char* pPathID = 0) { return filesystem->FileExists(pFileName, pPathID); }
	virtual bool			IsFileWritable(char const* pFileName, const char* pPathID = 0) { return filesystem->IsFileWritable(pFileName, pPathID); }
	virtual bool			SetFileWritable(char const* pFileName, bool writable, const char* pPathID = 0) { return filesystem->SetFileWritable(pFileName, writable, pPathID); }

	virtual long			GetFileTime(const char* pFileName, const char* pPathID = 0) { return filesystem->GetFileTime(pFileName, pPathID); }

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	virtual bool			ReadFile(const char* pFileName, const char* pPath, CUtlBuffer& buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL) { return filesystem->ReadFile(pFileName, pPath, buf, nMaxBytes, nStartingByte, pfnAlloc); }
	virtual bool			WriteFile(const char* pFileName, const char* pPath, CUtlBuffer& buf) { return filesystem->WriteFile(pFileName, pPath, buf); }
	virtual bool			UnzipFile(const char* pFileName, const char* pPath, const char* pDestination) { return filesystem->UnzipFile(pFileName, pPath, pDestination); }

public:
	//--------------------------------------------------------
	// Steam operations
	//--------------------------------------------------------

	virtual bool			IsSteam() const { return filesystem->IsSteam(); }

	// Supplying an extra app id will mount this app in addition 
	// to the one specified in the environment variable "steamappid"
	// 
	// If nExtraAppId is < -1, then it will mount that app ID only.
	// (Was needed by the dedicated server b/c the "SteamAppId" env var only gets passed to steam.dll
	// at load time, so the dedicated couldn't pass it in that way).
	virtual	FilesystemMountRetval_t MountSteamContent(int nExtraAppId = -1) { return filesystem->MountSteamContent(nExtraAppId); }

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath(const char* pPath, const char* pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL) { filesystem->AddSearchPath(pPath, pathID, addType); }
	virtual bool			RemoveSearchPath(const char* pPath, const char* pathID = 0) { return filesystem->RemoveSearchPath(pPath, pathID); }

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths(void) { filesystem->RemoveAllSearchPaths(); }

	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths(const char* szPathID) { filesystem->RemoveSearchPaths(szPathID); }

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly(const char* pPathID, bool bRequestOnly) { filesystem->MarkPathIDByRequestOnly(pPathID, bRequestOnly); }

	// converts a partial path into a full path
	virtual const char* RelativePathToFullPath(const char* pFileName, const char* pPathID, char* pLocalPath, int localPathBufferSize, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t* pPathType = NULL) { return filesystem->RelativePathToFullPath(pFileName, pPathID, pLocalPath, localPathBufferSize, pathFilter, pPathType); }

	// Returns the search path, each path is separated by ;s. Returns the length of the string returned
	virtual int				GetSearchPath(const char* pathID, bool bGetPackFiles, char* pPath, int nMaxLen) { return filesystem->GetSearchPath(pathID, bGetPackFiles, pPath, nMaxLen); }

	// interface for custom pack files > 4Gb
	virtual bool			AddPackFile(const char* fullpath, const char* pathID) { return filesystem->AddPackFile(fullpath, pathID); }

	//--------------------------------------------------------
	// File manipulation operations
	//--------------------------------------------------------

	// Deletes a file (on the WritePath)
	virtual void			RemoveFile(char const* pRelativePath, const char* pathID = 0) { filesystem->RemoveFile(pRelativePath, pathID); }

	// Renames a file (on the WritePath)
	virtual bool			RenameFile(char const* pOldPath, char const* pNewPath, const char* pathID = 0) { return filesystem->RenameFile(pOldPath, pNewPath, pathID); }

	// create a local directory structure
	virtual void			CreateDirHierarchy(const char* path, const char* pathID = 0) { filesystem->CreateDirHierarchy(path, pathID); }

	// File I/O and info
	virtual bool			IsDirectory(const char* pFileName, const char* pathID = 0) { return filesystem->IsDirectory(pFileName, pathID); }

	virtual void			FileTimeToString(char* pStrip, int maxCharsIncludingTerminator, long fileTime) { filesystem->FileTimeToString(pStrip, maxCharsIncludingTerminator, fileTime); }

	//--------------------------------------------------------
	// Open file operations
	//--------------------------------------------------------

	virtual void			SetBufferSize(FileHandle_t file, unsigned nBytes) { filesystem->SetBufferSize(file, nBytes); }

	virtual bool			IsOk(FileHandle_t file) { return filesystem->IsOk(file); }

	virtual bool			EndOfFile(FileHandle_t file) { return filesystem->EndOfFile(file); }

	virtual char* ReadLine(char* pOutput, int maxChars, FileHandle_t file) { return filesystem->ReadLine(pOutput, maxChars, file); }
	virtual int				FPrintf(FileHandle_t file, const char* pFormat, ...) FMTFUNCTION(3, 4) { return filesystem->FPrintf(file, pFormat); } // DOES NOT WORK FOR OBV REASONS

	//--------------------------------------------------------
	// Dynamic library operations
	//--------------------------------------------------------

	// load/unload modules
	virtual CSysModule* LoadModule(const char* pFileName, const char* pPathID = 0, bool bValidatedDllOnly = true) { return filesystem->LoadModule(pFileName, pPathID, bValidatedDllOnly); }
	virtual void			UnloadModule(CSysModule* pModule) { filesystem->UnloadModule(pModule); }

	//--------------------------------------------------------
	// File searching operations
	//--------------------------------------------------------

	// FindFirst/FindNext. Also see FindFirstEx.
	virtual const char* FindFirst(const char* pWildCard, FileFindHandle_t* pHandle) { return filesystem->FindFirst(pWildCard, pHandle); }
	virtual const char* FindNext(FileFindHandle_t handle) { return filesystem->FindNext(handle); }
	virtual bool			FindIsDirectory(FileFindHandle_t handle) { return filesystem->FindIsDirectory(handle); }
	virtual void			FindClose(FileFindHandle_t handle) { filesystem->FindClose(handle); }

	// Same as FindFirst, but you can filter by path ID, which can make it faster.
	virtual const char* FindFirstEx(
		const char* pWildCard,
		const char* pPathID,
		FileFindHandle_t* pHandle
	) {
		return filesystem->FindFirstEx(pWildCard, pPathID, pHandle);
	}

	//--------------------------------------------------------
	// File name and directory operations
	//--------------------------------------------------------

	// FIXME: This method is obsolete! Use RelativePathToFullPath instead!
	// converts a partial path into a full path
	virtual const char* GetLocalPath(const char* pFileName, char* pLocalPath, int localPathBufferSize) { return filesystem->GetLocalPath(pFileName, pLocalPath, localPathBufferSize); }

	// Returns true on success ( based on current list of search paths, otherwise false if 
	//  it can't be resolved )
	virtual bool			FullPathToRelativePath(const char* pFullpath, char* pRelative, int maxlen) { return filesystem->FullPathToRelativePath(pFullpath, pRelative, maxlen); }

	// Gets the current working directory
	virtual bool			GetCurrentDirectory(char* pDirectory, int maxlen) { return filesystem->GetCurrentDirectory(pDirectory, maxlen); }

	//--------------------------------------------------------
	// Filename dictionary operations
	//--------------------------------------------------------

	virtual FileNameHandle_t	FindOrAddFileName(char const* pFileName) { return filesystem->FindOrAddFileName(pFileName); }
	virtual bool				String(const FileNameHandle_t& handle, char* buf, int buflen) { return filesystem->String(handle, buf, buflen); }

	//--------------------------------------------------------
	// Asynchronous file operations
	//--------------------------------------------------------

	//------------------------------------
	// Global operations
	//------------------------------------
			FSAsyncStatus_t	_AsyncRead(const FileAsyncRequest_t& request, FSAsyncControl_t* phControl = NULL) { return AsyncReadMultiple(&request, 1, phControl); }
	virtual FSAsyncStatus_t	AsyncReadMultiple(const FileAsyncRequest_t* pRequests, int nRequests, FSAsyncControl_t* phControls = NULL) { return filesystem->AsyncReadMultiple(pRequests, nRequests, phControls); }
	virtual FSAsyncStatus_t	AsyncAppend(const char* pFileName, const void* pSrc, int nSrcBytes, bool bFreeMemory, FSAsyncControl_t* pControl = NULL) { return filesystem->AsyncAppend(pFileName, pSrc, nSrcBytes, bFreeMemory, pControl); }
	virtual FSAsyncStatus_t	AsyncAppendFile(const char* pAppendToFileName, const char* pAppendFromFileName, FSAsyncControl_t* pControl = NULL) { return filesystem->AsyncAppendFile(pAppendFromFileName, pAppendFromFileName, pControl); }
	virtual void			AsyncFinishAll(int iToPriority = 0) { filesystem->AsyncFinishAll(iToPriority); }
	virtual void			AsyncFinishAllWrites() { filesystem->AsyncFinishAllWrites(); }
	virtual FSAsyncStatus_t	AsyncFlush() { return filesystem->AsyncFlush(); }
	virtual bool			AsyncSuspend() { return filesystem->AsyncSuspend(); }
	virtual bool			AsyncResume() { return filesystem->AsyncResume(); }

	//------------------------------------
	// Functions to hold a file open if planning on doing mutiple reads. Use is optional,
	// and is taken only as a hint
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncBeginRead(const char* pszFile, FSAsyncFile_t* phFile) { return filesystem->AsyncBeginRead(pszFile, phFile); }
	virtual FSAsyncStatus_t	AsyncEndRead(FSAsyncFile_t hFile) { return filesystem->AsyncEndRead(hFile); }

	//------------------------------------
	// Request management
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncFinish(FSAsyncControl_t hControl, bool wait = true) { return filesystem->AsyncFinish(hControl, wait); }
	virtual FSAsyncStatus_t	AsyncGetResult(FSAsyncControl_t hControl, void** ppData, int* pSize) { return filesystem->AsyncGetResult(hControl, ppData, pSize); }
	virtual FSAsyncStatus_t	AsyncAbort(FSAsyncControl_t hControl) { return filesystem->AsyncAbort(hControl); }
	virtual FSAsyncStatus_t	AsyncStatus(FSAsyncControl_t hControl) { return filesystem->AsyncStatus(hControl); }
	// set a new priority for a file already in the queue
	virtual FSAsyncStatus_t	AsyncSetPriority(FSAsyncControl_t hControl, int newPriority) { return filesystem->AsyncSetPriority(hControl, newPriority); }
	virtual void			AsyncAddRef(FSAsyncControl_t hControl) { filesystem->AsyncAddRef(hControl); }
	virtual void			AsyncRelease(FSAsyncControl_t hControl) { filesystem->AsyncRelease(hControl); }

	//--------------------------------------------------------
	// Remote resource management
	//--------------------------------------------------------

	// starts waiting for resources to be available
	// returns FILESYSTEM_INVALID_HANDLE if there is nothing to wait on
	virtual WaitForResourcesHandle_t WaitForResources(const char* resourcelist) { return filesystem->WaitForResources(resourcelist); }
	// get progress on waiting for resources; progress is a float [0, 1], complete is true on the waiting being done
	// returns false if no progress is available
	// any calls after complete is true or on an invalid handle will return false, 0.0f, true 
	virtual bool			GetWaitForResourcesProgress(WaitForResourcesHandle_t handle, float* progress /* out */, bool* complete /* out */) { return filesystem->GetWaitForResourcesProgress(handle, progress, complete); }
	// cancels a progress call
	virtual void			CancelWaitForResources(WaitForResourcesHandle_t handle) { filesystem->CancelWaitForResources(handle); }

	// hints that a set of files will be loaded in near future
	// HintResourceNeed() is not to be confused with resource precaching.
	virtual int				HintResourceNeed(const char* hintlist, int forgetEverything) { return filesystem->HintResourceNeed(hintlist, forgetEverything); }
	// returns true if a file is on disk
	virtual bool			IsFileImmediatelyAvailable(const char* pFileName) { return IsFileImmediatelyAvailable(pFileName); }

	// copies file out of pak/bsp/steam cache onto disk (to be accessible by third-party code)
	virtual void			GetLocalCopy(const char* pFileName) { filesystem->GetLocalCopy(pFileName); }

	//--------------------------------------------------------
	// Debugging operations
	//--------------------------------------------------------

	// Dump to printf/OutputDebugString the list of files that have not been closed
	virtual void			PrintOpenedFiles(void) { filesystem->PrintOpenedFiles(); }
	virtual void			PrintSearchPaths(void) { filesystem->PrintSearchPaths(); }

	// output
	virtual void			SetWarningFunc(void (*pfnWarning)(const char* fmt, ...)) { filesystem->SetWarningFunc(pfnWarning); }
	virtual void			SetWarningLevel(FileWarningLevel_t level) { filesystem->SetWarningLevel(level); }
	virtual void			AddLoggingFunc(void (*pfnLogFunc)(const char* fileName, const char* accessType)) { AddLoggingFunc(pfnLogFunc); }
	virtual void			RemoveLoggingFunc(FileSystemLoggingFunc_t logFunc) { filesystem->RemoveLoggingFunc(logFunc); }

	// Returns the file system statistics retreived by the implementation.  Returns NULL if not supported.
	virtual const FileSystemStatistics* GetFilesystemStatistics() { return filesystem->GetFilesystemStatistics(); }

	//--------------------------------------------------------
	// Start of new functions after Lost Coast release (7/05)
	//--------------------------------------------------------

	virtual FileHandle_t	OpenEx(const char* pFileName, const char* pOptions, unsigned flags = 0, const char* pathID = 0, char** ppszResolvedFilename = NULL) { return filesystem->OpenEx(pFileName, pOptions, flags, pathID, ppszResolvedFilename); }

	// Extended version of read provides more context to allow for more optimal reading
	virtual int				ReadEx(void* pOutput, int sizeDest, int size, FileHandle_t file) { return filesystem->ReadEx(pOutput, sizeDest, size, file); }
	virtual int				ReadFileEx(const char* pFileName, const char* pPath, void** ppBuf, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL) { return filesystem->ReadFileEx(pFileName, pPath, ppBuf, bNullTerminate, bOptimalAlloc, nMaxBytes, nStartingByte, pfnAlloc); }

	virtual FileNameHandle_t	FindFileName(char const* pFileName) { return filesystem->FindFileName(pFileName); }

#if defined( TRACK_BLOCKING_IO )
	virtual void			EnableBlockingFileAccessTracking(bool state) = 0;
	virtual bool			IsBlockingFileAccessEnabled() const = 0;

	virtual IBlockingFileItemList* RetrieveBlockingFileAccessInfo() = 0;
#endif

	virtual void SetupPreloadData() { return filesystem->SetupPreloadData(); }
	virtual void DiscardPreloadData() { return filesystem->DiscardPreloadData(); }

	// Fixme, we could do these via a string embedded into the compiled data, etc...
	enum KeyValuesPreloadType_t
	{
		TYPE_VMT,
		TYPE_SOUNDEMITTER,
		TYPE_SOUNDSCAPE,
		NUM_PRELOAD_TYPES
	};

	// If the "PreloadedData" hasn't been purged, then this'll try and instance the KeyValues using the fast path of compiled keyvalues loaded during startup.
	// Otherwise, it'll just fall through to the regular KeyValues loading routines
	virtual KeyValues* LoadKeyValues(KeyValuesPreloadType_t type, char const* filename, char const* pPathID = 0) { return filesystem->LoadKeyValues((IFileSystem::KeyValuesPreloadType_t)type, filename, pPathID); }
	virtual bool		LoadKeyValues(KeyValues& head, KeyValuesPreloadType_t type, char const* filename, char const* pPathID = 0) { return filesystem->LoadKeyValues(head, (IFileSystem::KeyValuesPreloadType_t)type, filename, pPathID); }

	virtual FSAsyncStatus_t	AsyncWrite(const char* pFileName, const void* pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t* pControl = NULL) { return filesystem->AsyncWrite(pFileName, pSrc, nSrcBytes, bFreeMemory, bAppend, pControl); }
	virtual FSAsyncStatus_t	AsyncWriteFile(const char* pFileName, const CUtlBuffer* pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t* pControl = NULL) { return filesystem->AsyncWriteFile(pFileName, pSrc, nSrcBytes, bFreeMemory, bAppend, pControl); }
	// Async read functions with memory blame
	FSAsyncStatus_t			AsyncReadCreditAlloc(const FileAsyncRequest_t& request, const char* pszFile, int line, FSAsyncControl_t* phControl = NULL) { return AsyncReadMultipleCreditAlloc(request, 1, pszFile, line, phControl); }
	virtual FSAsyncStatus_t	AsyncReadMultipleCreditAlloc(const FileAsyncRequest_t& pRequests, int nRequests, const char* pszFile, int line, FSAsyncControl_t* phControls = NULL) { return filesystem->AsyncReadCreditAlloc(pRequests, pszFile, line, phControls); }

	virtual FSAsyncStatus_t AsyncDirectoryScan(const char* pSearchSpec, bool recurseFolders, void* pContext, FSAsyncScanAddFunc_t pfnAdd, FSAsyncScanCompleteFunc_t pfnDone, FSAsyncControl_t* pControl = NULL) { return filesystem->AsyncDirectoryScan(pSearchSpec, recurseFolders, pContext, pfnAdd, pfnDone, pControl); }

	virtual bool			GetFileTypeForFullPath(char const* pFullPath, wchar_t* buf, size_t bufSizeInBytes) { return filesystem->GetFileTypeForFullPath(pFullPath, buf, bufSizeInBytes); }

	//--------------------------------------------------------
	//--------------------------------------------------------
	virtual bool		ReadToBuffer(FileHandle_t hFile, CUtlBuffer& buf, int nMaxBytes = 0, FSAllocFunc_t pfnAlloc = NULL) { return filesystem->ReadToBuffer(hFile, buf, nMaxBytes, pfnAlloc); }

	//--------------------------------------------------------
	// Optimal IO operations
	//--------------------------------------------------------
	virtual bool		GetOptimalIOConstraints(FileHandle_t hFile, unsigned* pOffsetAlign, unsigned* pSizeAlign, unsigned* pBufferAlign) { return filesystem->GetOptimalIOConstraints(hFile, pOffsetAlign, pSizeAlign, pBufferAlign); }
	inline unsigned		GetOptimalReadSize(FileHandle_t hFile, unsigned nLogicalSize) { return filesystem->GetOptimalReadSize(hFile, nLogicalSize); }
	virtual void* AllocOptimalReadBuffer(FileHandle_t hFile, unsigned nSize = 0, unsigned nOffset = 0) { return filesystem->AllocOptimalReadBuffer(hFile, nSize, nOffset); }
	virtual void		FreeOptimalReadBuffer(void* a) { filesystem->FreeOptimalReadBuffer(a); }

	//--------------------------------------------------------
	//
	//--------------------------------------------------------
	virtual void		BeginMapAccess() { filesystem->BeginMapAccess(); }
	virtual void		EndMapAccess() { filesystem->EndMapAccess(); }

	// Returns true on success, otherwise false if it can't be resolved
	virtual bool		FullPathToRelativePathEx(const char* pFullpath, const char* pPathId, char* pRelative, int maxlen) { return filesystem->FullPathToRelativePath(pFullpath, pRelative, maxlen); }

	virtual int			GetPathIndex(const FileNameHandle_t& handle) { return filesystem->GetPathIndex(handle); }
	virtual long		GetPathTime(const char* pPath, const char* pPathID) { return filesystem->GetPathTime(pPath, pPathID); }

	virtual DVDMode_t	GetDVDMode() { return filesystem->GetDVDMode(); }

	//--------------------------------------------------------
	// Whitelisting for pure servers.
	//--------------------------------------------------------

	// This should be called ONCE at startup. Multiplayer games (gameinfo.txt does not contain singleplayer_only)
	// want to enable this so sv_pure works.
	virtual void			EnableWhitelistFileTracking(bool bEnable) { filesystem->EnableWhitelistFileTracking(bEnable); }

	// This is called when the client connects to a server using a pure_server_whitelist.txt file.
	//
	// Files listed in pWantCRCList will have CRCs calculated for them IF they come off disk
	// (and those CRCs will come out of GetUnverifiedCRCFiles).
	//
	// Files listed in pAllowFromDiskList will be allowed to load from disk. All other files will
	// be forced to come from Steam.
	//
	// The filesystem hangs onto the whitelists you pass in here, and it will Release() them when it closes down
	// or when you call this function again.
	//
	// NOTE: The whitelists you pass in here will be accessed from multiple threads, so make sure the 
	//       IsFileInList function is thread safe.
	//
	// If pFilesToReload is non-null, the filesystem will hand back a list of files that should be reloaded because they
	// are now "dirty". For example, if you were on a non-pure server and you loaded a certain model, and then you connected
	// to a pure server that said that model had to come from Steam, then pFilesToReload would specify that model
	// and the engine should reload it so it can come from Steam.
	//
	// Be sure to call Release() on pFilesToReload.
	virtual void			RegisterFileWhitelist(IFileList* pWantCRCList, IFileList* pAllowFromDiskList, IFileList** pFilesToReload) { filesystem->RegisterFileWhitelist(pWantCRCList, pAllowFromDiskList, pFilesToReload); }

	// Called when the client logs onto a server. Any files that came off disk should be marked as 
	// unverified because this server may have a different set of files it wants to guarantee.
	virtual void			MarkAllCRCsUnverified() { filesystem->MarkAllCRCsUnverified(); }

	// As the server loads whitelists when it transitions maps, it calls this to calculate CRCs for any files marked
	// with check_crc.   Then it calls CheckCachedFileCRC later when it gets client requests to verify CRCs.
	virtual void			CacheFileCRCs(const char* pPathname, ECacheCRCType eType, IFileList* pFilter) { filesystem->CacheFileCRCs(pPathname, eType, pFilter); }
	virtual EFileCRCStatus	CheckCachedFileCRC(const char* pPathID, const char* pRelativeFilename, CRC32_t* pCRC) { return filesystem->CheckCachedFileCRC(pPathID, pRelativeFilename, pCRC); }

	// Fills in the list of files that have been loaded off disk and have not been verified.
	// Returns the number of files filled in (between 0 and nMaxFiles).
	//
	// This also removes any files it's returning from the unverified CRC list, so they won't be
	// returned from here again.
	// The client sends batches of these to the server to verify.
	virtual int				GetUnverifiedCRCFiles(CUnverifiedCRCFile* pFiles, int nMaxFiles) { return filesystem->GetUnverifiedCRCFiles(pFiles, nMaxFiles); }

	// Control debug message output.
	// Pass a combination of WHITELIST_SPEW_ flags.
	virtual int				GetWhitelistSpewFlags() { return filesystem->GetWhitelistSpewFlags(); }
	virtual void			SetWhitelistSpewFlags(int flags) { filesystem->SetWhitelistSpewFlags(flags); }

	// Installs a callback used to display a dirty disk dialog
	virtual void			InstallDirtyDiskReportFunc(FSDirtyDiskReportFunc_t func) { filesystem->InstallDirtyDiskReportFunc(func); }

	virtual bool			IsLaunchedFromXboxHDD() { return filesystem->IsLaunchedFromXboxHDD(); }
	virtual bool			IsInstalledToXboxHDDCache() { return filesystem->IsInstalledToXboxHDDCache(); }
	virtual bool			IsDVDHosted() { return filesystem->IsDVDHosted(); }
	virtual bool			IsInstallAllowed() { return filesystem->IsInstallAllowed(); }

	virtual int				GetSearchPathID(char* pPath, int nMaxLen) { return filesystem->GetSearchPathID(pPath, nMaxLen); }
	virtual bool			FixupSearchPathsAfterInstall() { return filesystem->FixupSearchPathsAfterInstall(); }

	virtual FSDirtyDiskReportFunc_t		GetDirtyDiskReportFunc() { return filesystem->GetDirtyDiskReportFunc(); }

	virtual void AddVPKFile(char const* pszName, SearchPathAdd_t addType = PATH_ADD_TO_TAIL) { filesystem->AddVPKFile(pszName, addType); }
	virtual void RemoveVPKFile(char const* pszName) { filesystem->RemoveVPKFile(pszName); }
	virtual void GetVPKFileNames(CUtlVector<CUtlString>& destVector) { filesystem->GetVPKFileNames(destVector); }
	virtual void			RemoveAllMapSearchPaths() { filesystem->RemoveAllMapSearchPaths(); }
	virtual void			SyncDvdDevCache() { filesystem->SyncDvdDevCache(); }

	virtual bool			GetStringFromKVPool(CRC32_t poolKey, unsigned int key, char* pOutBuff, int buflen) { return filesystem->GetStringFromKVPool(poolKey, key, pOutBuff, buflen); }

	virtual bool			DiscoverDLC(int iController) { return filesystem->DiscoverDLC(iController); }
	virtual int				IsAnyDLCPresent(bool* pbDLCSearchPathMounted = NULL) { return filesystem->IsAnyDLCPresent(pbDLCSearchPathMounted); }
	virtual bool			GetAnyDLCInfo(int iDLC, unsigned int* pLicenseMask, wchar_t* pTitleBuff, int nOutTitleSize) { return filesystem->GetAnyDLCInfo(iDLC, pLicenseMask, pTitleBuff, nOutTitleSize); }
	virtual int				IsAnyCorruptDLC() { return filesystem->IsAnyCorruptDLC(); }
	virtual bool			GetAnyCorruptDLCInfo(int iCorruptDLC, wchar_t* pTitleBuff, int nOutTitleSize) { return filesystem->GetAnyCorruptDLCInfo(iCorruptDLC, pTitleBuff, nOutTitleSize); }
	virtual bool			AddDLCSearchPaths() { return filesystem->AddDLCSearchPaths(); }
	virtual bool			IsSpecificDLCPresent(unsigned int nDLCPackage) { return filesystem->IsSpecificDLCPresent(nDLCPackage); }

	// call this to look for CPU-hogs during loading processes. When you set this, a breakpoint
	// will be issued whenever the indicated # of seconds go by without an i/o request.  Passing
	// 0.0 will turn off the functionality.
	virtual void            SetIODelayAlarm(float flThreshhold) { filesystem->SetIODelayAlarm(flThreshhold); }

};

extern IFileSysPrx* g_pFullFileSysPrx;