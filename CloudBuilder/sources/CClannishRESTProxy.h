//
//  CClannishDBProxy.h
//  CloudBuilder
//
//  Created by Roland Van Leeuwen on 15/09/11.
//  Copyright 2011 Clan of the Cloud. All rights reserved.
//

#ifndef CotClib_CConnectionREST_h
#define CotClib_CConnectionREST_h

#include <sys/types.h>
#include <vector>
#include "CloudBuilder.h"
#include "CCallback.h"
#include "helpers.h"
#include "curltool.h"
#include "CClan.h"
#include "CUserManager.h"

#define ADMIN_EVENT_DOMAIN "private"

using namespace CotCHelpers;

namespace CloudBuilder {
	class CHDate;
	class CClan;
	
	typedef enum  etTypeFS {
		fsApp = 1,
		fsAppUser,
		fsAppMatch
	} etTypeFS;

	class CClannishRESTProxy {
	public:
		
		static CClannishRESTProxy *Instance();

		bool isSetup();
		bool isLoggedIn();
		void SetNetworkState(bool on);
		
		void Ping(CInternalResultHandler *onFinished);
		
		void Setup(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void Login(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void LoginAnonymous(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void LogWithExternalNetwork(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void Logout(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void Convert(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void MailPassword(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void ChangePassword(const char *aNewPassword, CInternalResultHandler *onFinished);
		void ChangeEmail(const char *aNewEmail, CInternalResultHandler *onFinished);

		void BatchGame(const CHJSON *ajSON, const CHJSON *aInput, CInternalResultHandler *onFinished);
		void BatchUser(const CHJSON *ajSON, const CHJSON *aInput, CInternalResultHandler *onFinished);

		void CheckUser(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void UserExist(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void ListUsers(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		
		void ListFriends(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void BlacklistFriends(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void ChangeRelationshipStatus(const CHJSON *ajSON, CInternalResultHandler *onFinished);

		void RegisterDevice(const char *os, const char *token, CInternalResultHandler *onFinished);
		void UnregisterDevice(const char *os, const char *token, CInternalResultHandler *onFinished);

		void Outline(CInternalResultHandler *onFinished);

		void GetGodfatherCode(const char *aDomain, CInternalResultHandler *onFinished);
		void GetGodfather(const char *aDomain, CInternalResultHandler *onFinished);
		void SetGodfather(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void GetGodchildren(const char *aDomain, CInternalResultHandler *onFinished);
	
		void GetUserProfile(const CHJSON *ajSON, CInternalResultHandler *onFinished);
		void SetUserProfile(const CHJSON *aJSON, CInternalResultHandler *onFinished);

		void UserSetProperties(const char *aDomain, const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void UserGetProperties(const char *aDomain, CInternalResultHandler *onFinished);
		void UserSetProperty(const char *aDomain, const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void UserGetProperty(const char *aDomain, const char *key, CInternalResultHandler *onFinished);
		void UserDelProperty(const char *aDomain, const char *key, CInternalResultHandler *onFinished);
		void FindOpponents(const char *aDomain, const CHJSON *ajSON,  CInternalResultHandler *onFinished);

		/**
		 * @param aJSON configuration for the transaction (directly forwarded to the /v2.2/gamer/tx API call)
		 * @param useV2_2 should be set to true, else will perform the call on the legacy resource (v1)
		 * @param onFinished handler for the result (which contents will differ depending on whether useV2_2 was passed)
		 */
		void Transaction(const CHJSON *aJSON, bool useV2_2, CInternalResultHandler *onFinished);
		void Balance(const char *domain, const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void TxHistory(const char *domain, const CHJSON *aJSON, CInternalResultHandler *onFinished);

        void vfsRead(const char *domain, const char *key, CInternalResultHandler *onFinished);
        void vfsWrite(const char *domain, const char *key, const CHJSON *aJSON, bool isBinary, CInternalResultHandler *onFinished);
        void vfsReadv3(const char *domain, const char *key, CInternalResultHandler *onFinished);
        void vfsWritev3(const char *domain, const char *key, const CHJSON *aJSON, bool isBinary, CInternalResultHandler *onFinished);
		void vfsDelete(const char *domain, const char *key, bool isBinary, CInternalResultHandler *onFinished);
		void UploadData(const char *url, const void *ptr, size_t size, CInternalResultHandler *onFinished);
		void DownloadData(const char *url, CInternalResultHandler *onFinished);
	   
		void vfsReadGame(const char *domain, const char *key, CInternalResultHandler *onFinished);
		void vfsWriteGame(const char *domain, const char *key, const CHJSON *aJSON, bool isBinary, CInternalResultHandler *onFinished);
		void vfsDeleteGame(const char *domain, const char *key, bool isBinary, CInternalResultHandler *onFinished);

		void Score(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void FriendsBestHighScore(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void GetRank(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void BestHighScore(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void CenteredScore(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void UserBestScore(const char *domain, CInternalResultHandler *onFinished);
		void GetRankArray(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void BestHighScoreArray(const CHJSON *aJSON, CInternalResultHandler *onFinished);

		void LinkWith(const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void Unlink(const CHJSON *aJSON, CInternalResultHandler *onFinished);

		void GetNetworkFriends(const char *network, const CHJSON *ajSON, CInternalResultHandler *onFinished);

		void InvitByMail(const CHJSON *ajSON, CInternalResultHandler *onFinished);

		// configuration should contain: domain
		void ListAchievements(const CHJSON *configuration, CInternalResultHandler *onFinished);
		void SetAchievementGamerData(const char *domain, const char *achName, const CHJSON *data, CInternalResultHandler *onFinished);
		void PushEvent(const char *domain, const char *gamerid, const CHJSON *aJSON, CInternalResultHandler *onFinished);
		void ResetAchievements(const char *domain, CInternalResultHandler *onFinished);

		CCloudResult *ForceActivate();

		// Match API
		void CreateMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void FinishMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void DeleteMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void JoinMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void InvitePlayerToMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void DismissInvitationToMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void DrawFromShoeInMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void LeaveMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void FetchMatch(const CHJSON *config, CInternalResultHandler *onFinished);
		void ListMatches(const CHJSON *config, CInternalResultHandler *onFinished);
		void PostMove(const CHJSON *config, CInternalResultHandler *onFinished);
		
		// Store API
		void GetProductList(const CHJSON *config, CInternalResultHandler *onFinished);
		void GetPurchaseHistory(const CHJSON *config, CInternalResultHandler *onFinished);
		void SendAppStorePurchaseToServer(const CHJSON *config, CInternalResultHandler *onFinished);

		// Index API
		void DeleteIndexedObject(const CHJSON *config, CInternalResultHandler *onFinished);
		void GetIndexedObject(const CHJSON *config, CInternalResultHandler *onFinished);
		void IndexObject(const CHJSON *config, CInternalResultHandler *onFinished);
		void SearchIndexedObjects(const CHJSON *config, CInternalResultHandler *onFinished);

		/**
		 * Registers an event listener for a given event domain.
		 * @param domain domain to listen for, such as "/private" (use NULL for default)
		 * @param listener listener to receive events
		 * Note about memory management: when you inherit from CEventListener, your class becomes a
		 * CRefClass. The system will keep a reference to it while registered, and call Release when
		 * it is unregistered. Thus, if you want the object to be released automatically when
		 * unregistered, simply call Release() on it:
			CEventListener *listener = new MyEventListener;
			
		 */
		eErrorCode RegisterEventListener(const char *domain, CEventListener *listener);
		/**
		 * Unregisters an event listener, killing the associated thread when no more listeners are left for a given domain.
		 * @param domain the domain for which the listener was registered
		 * @param listener the listener to remove (Release will be called on it, potentially deleting it)
		 * @param acquireLock internal parameter, leave it to true when using from outside
		 */
		eErrorCode UnregisterEventListener(const char *domain, CEventListener *listener, bool acquireLock = true);
		void StartEventListening();
		void StopEventListening();
		
		void Suspend();
		void Resume();

		const char *GetGamerID();
		const char *GetNetworkID();
		const char *GetNetwork();
		const char *GetDisplayName();
		const char *GetMail();
		const char *GetAppID();
		
		bool isLinkedWith(const char *network) { return IsEqual(mNetwork, network) || mLinks->Has(network);  }

		bool autoRegisterForNotification() { return mRegisterForNotification; }
		
	private:
		// Background thread issuing "pop" commands to the server.
		class PopEventLoopThread;
		CMutex popEventThreadMutex;
		CConditionVariable suspendedThreadLock;

		/** singleton */
		CHJSON *mQueuesURL;
		CHJSON *mJSONBatch;

		cstring mApiKey, mApiSecret;
		cstring mGamerId, mGamerSecret;
		cstring mNetwork, mNetworkId;
		cstring mDisplayName, mEmail;
		cstring mAppID, mAppVersion, mSdkVersion;
		std::vector< autoref<PopEventLoopThread> > popEventThreads;
		int mPopEventLoopDelay;			// in sec

		CHJSON *mLinks;
		
		bool mNetSate, mSuspend;
		bool mRegisterForNotification;
		
		friend struct singleton_holder<CClannishRESTProxy>;
		friend void CClan::Terminate();

		// Can not be subclassed
		CClannishRESTProxy();
		virtual ~CClannishRESTProxy();
		/**
		 * You need to call this in CClan::Terminate().
		 */
		void Terminate();

		bool HandleEvent(const CHJSON *ajSON);
		// Please acquire popEventThreadMutex before calling this!
		PopEventLoopThread *FindEventThread(const char *domain);

		CCloudResult *LoginResultHandler(CCloudResult *result);
		CCloudResult *LogoutResultHandler(CCloudResult *result);

		/**
		 * From a configuration JSON that potentially has an OSN field, builds a body containing only this node.
 		 * @param config configuration (that may be null), possibly containing an `osn` node
		 * @return either NULL or a CHJSON with only the `osn` node from the original `config`  parameter
		 */
		CHJSON *MakeBodyWithOsn(const CHJSON *config);

		/**
		 * Builds an HTTP request targetting our server, without adding credentials.
		 * @param url URL relative to the server (e.g. /api/login)
		 */
		CHttpRequest *MakeUnauthenticatedHttpRequest(const char *url);
		/**
		 * Builds an HTTP request targetting our server, adding in the credentials if needed.
		 * @param url URL relative to the server (e.g. /api/login)
		 */
		CHttpRequest *MakeHttpRequest(const char *url);
	};
	
}

#endif
