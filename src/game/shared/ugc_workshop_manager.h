//========= Copyright (c) Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//============================================================================//

#ifndef _WORKSHOP_MANAGER_H__
#define _WORKSHOP_MANAGER_H__

#include "..\..\public\steam\isteamremotestorage.h"
#include "steam/steam_api.h"
#include "filesystem.h"
//#include "ugc_request_manager.h"
//#include "ugc_file_info_manager.h"

DECLARE_LOGGING_CHANNEL(LOG_WORKSHOP);

#if !defined( NO_STEAM )

extern ISteamRemoteStorage* GetISteamRemoteStorage();

struct PublishedFileInfo_t
{
	PublishedFileInfo_t(PublishedFileId_t nID = 0) :
		m_nPublishedFileId(nID),
		m_hFile(k_UGCHandleInvalid),
		m_hPreviewFile(k_UGCHandleInvalid),
		m_ulSteamIDOwner(0),
		m_rtimeCreated(0),
		m_rtimeUpdated(0),
		m_eVisibility(k_ERemoteStoragePublishedFileVisibilityPublic),
		m_rtimeSubscribed(0),
		m_rtimeLastPlayed(0),
		m_rtimeCompleted(0),
		m_unUpVotes(0),
		m_unDownVotes(0),
		m_flVotingScore(0.0f),
		m_bVotingDataValid(false),
		m_unNumReports(0)
	{
		memset(m_rgchTitle, 0, ARRAYSIZE(m_rgchTitle));
		memset(m_rgchDescription, 0, ARRAYSIZE(m_rgchDescription));
		memset(m_pchFileName, 0, ARRAYSIZE(m_pchFileName));
	}

	friend class CWorkshopFileInfoManager;

	// Basic info
	PublishedFileId_t m_nPublishedFileId;
	char m_rgchTitle[k_cchPublishedDocumentTitleMax];
	UGCHandle_t m_hFile;
	UGCHandle_t m_hPreviewFile;
	uint64 m_ulSteamIDOwner;
	uint32 m_rtimeCreated;
	uint32 m_rtimeUpdated;
	ERemoteStoragePublishedFileVisibility m_eVisibility;
	uint32 m_rtimeSubscribed;
	uint32 m_rtimeLastPlayed;
	uint32 m_rtimeCompleted;
	char m_rgchDescription[k_cchPublishedDocumentDescriptionMax];
	char m_pchFileName[k_cchFilenameMax];
	char m_rgchTags[k_cchTagListMax];
	bool m_bTagsTruncated;
	CCopyableUtlVector<char*> m_vTags;

	// Whether or not the voting data has been properly collected for this item
	bool HasVoteData(void) const { return m_bVotingDataValid; }

	// Get the vote information for this item
	// NOTE: Because this isn't native to the published file, it must be requested separately. 
	//		 This function protects from the case where the data is not valid when requested (returns false)
	bool GetVoteData(float* pScore, uint32* pUpVotes, uint32* pDownVotes) const
	{
		// We must have real data here to allow a query for it
		if (HasVoteData() == false)
			return false;

		if (pScore != NULL)
		{
			*pScore = m_flVotingScore;
		}

		if (pUpVotes != NULL)
		{
			*pUpVotes = m_unUpVotes;
		}

		if (pDownVotes != NULL)
		{
			*pDownVotes = m_unDownVotes;
		}

		return true;
	}

	bool HasTag(const char* pszTag) const
	{
		for (int i = 0; i < m_vTags.Count(); ++i)
		{
			if (!V_stricmp(m_vTags[i], pszTag))
			{
				return true;
			}
		}

		return false;
	}

private:

	// Voting info
	bool	m_bVotingDataValid;	// If this isn't true, then no request has been done to retrieve it
	uint32	m_unUpVotes;
	uint32  m_unDownVotes;
	float	m_flVotingScore;
	uint32	m_unNumReports;		// FIXME: This isn't exposed anywhere, currently

};

class IWorkshopFileInfoManagerCallbackInterface
{
public:
	// File requests
	virtual void OnFileRequestFinished(UGCHandle_t hFileHandle) = 0;
	virtual void OnFileRequestError(UGCHandle_t hFileHandle) = 0;
};

class IWorkshopManagerCallbackInterface : public IWorkshopFileInfoManagerCallbackInterface
{
public:
	// Published files
	virtual void OnPublishedFileSubscribed( PublishedFileId_t nID ) = 0;
	virtual void OnPublishedFileUnsubscribed( PublishedFileId_t nID ) = 0;
	virtual void OnPublishedFileDeleted( PublishedFileId_t nID ) = 0;
};

class CBaseWorkshopManagerCallbackInterface : public IWorkshopManagerCallbackInterface
{
public:

	// File requests
	virtual void OnFileRequestFinished( UGCHandle_t hFileHandle ) {}
	virtual void OnFileRequestError( UGCHandle_t hFileHandle ) {}

	// Published files
	virtual void OnPublishedFileSubscribed( PublishedFileId_t nID ) {}
	virtual void OnPublishedFileUnsubscribed( PublishedFileId_t nID ) {}
	virtual void OnPublishedFileDeleted( PublishedFileId_t nID ) {}
};


enum UGCFileRequestStatus_t
{
	UGCFILEREQUEST_INVALID = -2,		// This was an invalid request for some reason
	UGCFILEREQUEST_ERROR = -1,			// An error occurred while processing the file operation
	UGCFILEREQUEST_READY,				// File request is ready to do work
	UGCFILEREQUEST_DOWNLOADING,			// Currently downloading a file
	UGCFILEREQUEST_UNZIPPING,			// Threaded unzipping
	UGCFILEREQUEST_DOWNLOAD_WRITING,	// Async write of the downloaded file to the disc
	UGCFILEREQUEST_UPLOADING,			// Currently uploading a file
	UGCFILEREQUEST_FINISHED				// Operation complete, no work waiting
};

class CBasePublishedFileRequest
{
public:
	CBasePublishedFileRequest(PublishedFileId_t targetID) :
		m_nTargetID(targetID)
	{}

	virtual void OnError(EResult nErrorCode) { /* Do nothing */ }
	virtual void OnLoaded(PublishedFileInfo_t& info) { /* Do nothing */ }

	PublishedFileId_t	GetTargetID(void) const { return m_nTargetID; }

private:
	PublishedFileId_t	m_nTargetID;			// File ID to get information about
};

class CWorkshopManager 
{

public:

	CWorkshopManager(IWorkshopManagerCallbackInterface* pCallbackInterface) {}
	~CWorkshopManager( void ) {}

	void Update(void) {}

#if !defined( NO_STEAM )

	//
	// PFI Depot
	//

	const PublishedFileInfo_t* GetPublishedFileInfoByID(PublishedFileId_t nID) const { return 0; }
	bool						IsFileInfoRequestStillPending(PublishedFileId_t nID) const { return false; }
	bool						AddFileInfoQuery(CBasePublishedFileRequest* pRequest, bool bAllowUpdate = false) { return false; }
	bool						AddPublishedFileVoteInfoRequest(const PublishedFileInfo_t* pInfo, bool bForceUpdate = false) { return false; }
	void						UpdatePublishedItemVote( PublishedFileId_t nFileID, bool bVoteUp ) {}

	bool						HasPendingDownloads(void) const { return false; }

	// 
	// UGC methods
	// 

	bool						DeletePublishedFile(PublishedFileId_t nID) { return false; }
	bool						DeleteUGCFileRequest(UGCHandle_t handle, bool bRemoveFromDisk = false) { return false; }
	bool						UGCFileRequestExists(UGCHandle_t handle) { return false; }
	void						GetUGCFullPath( UGCHandle_t handle, char *pDest, size_t nSize );
	const char* GetUGCFileDirectory(UGCHandle_t handle) { return ""; }
	const char* GetUGCFilename(UGCHandle_t handle) { return ""; }
	UGCFileRequestStatus_t		GetUGCFileRequestStatus(UGCHandle_t handle) { return UGCFileRequestStatus_t::UGCFILEREQUEST_INVALID; }
	bool						PromoteUGCFileRequestToTop(UGCHandle_t handle) { return false; }

	// These are special cases where th UGCHandle_t is yet unknown (as in the case of uploads)
	UGCHandle_t					GetUGCFileHandleByFilename(const char* lpszFilename) { return 0; }
	UGCFileRequestStatus_t		GetUGCFileRequestStatusByFilename(const char* lpszFilename) { return UGCFileRequestStatus_t::UGCFILEREQUEST_INVALID; }
	float						GetUGCFileDownloadProgress( UGCHandle_t handle ) const;

	bool						CreateFileDownloadRequest(	UGCHandle_t hFileHandle, 
															PublishedFileId_t fileID,
															const char *lpszDirectory, 
															const char *lpszFilename, 
															uint32 unPriority = 0, 
															uint32 unTimeLastUpdated = 0, 
		bool bForceUpdate = false) {
		return false;
	}

	bool						CreateFileUploadRequest(	const char *lpszSourceFilename,
															const char *lpszDirectory, 
															const char *lpszFilename, 
		uint32 unPriority = 0) {
		return false;
	}

#endif // !NO_STEAM

private:

	// Callback interface
	IWorkshopManagerCallbackInterface	*m_pCallbackInterface;

	//
	// Community map files
	//

	void UpdateUGCRequests( void ) {}

	// Callback for deleting files
	CCallResult<CWorkshopManager, RemoteStorageDeletePublishedFileResult_t> m_callbackDeletePublishedFile;
	void Steam_OnDeletePublishedFile( RemoteStorageDeletePublishedFileResult_t *pResult, bool bError ) {}

#if !defined( _GAMECONSOLE )
	void Steam_OnUpdateUserPublishedItemVote( RemoteStorageUpdateUserPublishedItemVoteResult_t *pResult, bool bError ) {}
	void Steam_OnFileSubscribed( RemoteStoragePublishedFileSubscribed_t *pCallback ) {}
	void Steam_OnFileUnsubscribed( RemoteStoragePublishedFileUnsubscribed_t *pCallback ) {}
	void QueryForCommunityMaps( void ) {}
	CCallResult<CWorkshopManager, RemoteStorageUpdateUserPublishedItemVoteResult_t> m_callbackUpdateUserPublishedItemVote;
	//CCallback< CWorkshopManager, RemoteStoragePublishedFileSubscribed_t, false> m_callbackFileSubscribed;
	//CCallback< CWorkshopManager, RemoteStoragePublishedFileUnsubscribed_t, false> m_callbackFileUnsubscribed;
#endif // !_GAMECONSOLE

	// 
	// File Information Manager
	//

	//CWorkshopFileInfoManager		m_WorkshopFileInfoManager;

	// 
	// UGC Request Manager
	//

	//CUGCFileRequestManager			m_UGCFileRequestManager;
	bool							m_bUGCRequestsPaused;

	int					m_nTotalSubscriptionsLoaded;				// Number of subscriptions we've received from the Steam server. This may not be the total number available, meaning we need to requery.
	bool				m_bReceivedQueueBaseline;					// Whether or not we've heard back successfully from Steam on our queue status
	float				m_flQueueBaselineRequestTime;				// Time that the baseline was requested from the GC

};

#endif // !NO_STEAM

#endif // _WORKSHOP_MANAGER_H__
