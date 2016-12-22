//
//  CUserManager.h
//  CloudBuilder
//
//  Created by Roland Van Leeuwen on 26/04/12.
//  Copyright (c) 2012 Clan of the Cloud. All rights reserved.
//

#ifndef CloudBuilder_CUserManager_h
#define CloudBuilder_CUserManager_h

/*! \file CUserManager.h
 */
#include "CotCHelpers.h"
#include "CHJSON.h"

extern "C" {
	void LaunchAuthenticate(void);
}

namespace CloudBuilder
{

	class CCloudResult;

	/** \cond INTERNAL_USE */
	void AchieveRegisterDevice(unsigned long len, const void *bytes);
	/** \endcond */

	/**
		Implement this to be able to receive notifications related to events.
		When one of these functions is called (either because of an error or success), the following
		information will be passed along:
		@param aErrorCode
		@param aDomain the domain on which the event is received
		@param aEvent a json formatted as :
		"event" : {} , the json passed to pushEvent method, free form , according to software need
		"user" : the gamer_id of the user who push the event.
		"name" : the displayName of the user who pushed the event.
		"id" : the id of the event, generated by the system.
	 
		If you implement this, beware that your class will become a CRefClass, meaning that you should not
		delete it, but call Release() when you do not need the instance anymore.
	 */
	struct FACTORY_CLS CEventListener: CotCHelpers::CRefClass {
		virtual void onEventReceived(const char *aDomain, const CCloudResult *aEvent) = 0;
		virtual void onEventError(eErrorCode aErrorCode, const char *aDomain, const CCloudResult *result) = 0;
	};

	/**
		The CloudBuilder::CUserManager is the second class you will use once you are connected with
		CloudBuilder::CClan::Setup method. This class manages a user profile.
		The class is a singleton and is not designed to be overridden. In version 2, we use the
		concept of delegates to get the results, in the same way it was designed in the C# wrapper.
	 */
	class FACTORY_CLS CUserManager {
	public:

		/**
			Use this method to obtain a reference to the CUserManager.
			@return the one and only instance of this manager
		*/
		static CUserManager *Instance();
		
		/** @defgroup user_login Login related tasks
		 * @{
		 */

		/**
			Method used to login using no identifer.
			This method will fail (enOperationAlreayInProgress) if any other login related operation is already launched.
            @param aOptions is a JSON object holding optional data, see CloudBuilder::CUserManager::LoginNetwork for more
            about possible options. Can be NULL if no options are to be passed.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"gamer_id" : "xxxxx",
			"gamer_secret" : "xxxxxx"
			"profile" : {}
		*/
        void LoginAnonymous(const CotCHelpers::CHJSON* aOptions, CResultHandler *aHandler);
        DEPRECATED void LoginAnonymous(CResultHandler *aHandler);
		
		/**
			Method used to log in with a with an account of a social network.
			This method will fail (enOperationAlreayInProgress) if any other login related operation is already launched.
			@param aConfiguration is a JSON object holding the necessary connection details.
			The mandatory keys are:
			- "network" : facebookId, googleplusId, gamecenter, email (facebook and goggleplus are deprecated)
			- "id" : string containing the ID of the user. (respectively facebook ID, googleplus ID, gamecenter ID, email)
			- "secret" : string containing the secret associated with the user ID. (respectivly accesstoken, token, n/s, password)
		 	optionally pass an 'options' key which may contain:
		 	- 'preventRegistration' : doesn't create a new account if not already existing. In this it can raise an
		 		error named 'PreventRegistration'.
            - 'thenBatch' : value should contain { "thenBatch" : { "domain" : "yourDomain" , "name" : "batchToCall" , 
                                                    "params" : { your JSON parameters } } }
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"gamer_id" : "xxxxx",
			"gamer_secret" : "xxxxxx"
			"profile" : {}
		*/
		void LoginNetwork(const CotCHelpers::CHJSON* aConfiguration, CResultHandler *aHandler);

		/**
			Method used to log in with a profile previously stored as the result of a LoginAnonymous or LoginNetwork.
			This method will fail (enOperationAlreayInProgress) if any other login related operation is already launched.
			@param aConfiguration is a JSON object holding the necessary connection details.
			The mandatory keys are:
			- "id" : string containing the ID of the user.
			- "secret" : string containing the secret associated with the user ID.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"gamer_id" : "xxxxx",
			"gamer_secret" : "xxxxxx"
			"profile" : {}
		*/
		void ResumeSession(const CotCHelpers::CHJSON* aConfiguration, CResultHandler *aHandler);
			
		/**
			Method used to log out a previously logged in profile. After calling this, you need to
			log again using any of LoginAnonymous, LoginNetwork or Login before being able
			to call other methods.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"done" : 1
		*/
		void Logout(CResultHandler *aHandler);
		
		/**
			Method used to Link an account with Facebook, Googleplus, GameCenter.
		 	@param aConfiguration is a JSON object holding the necessary connection details
			The mandatory keys are:
			- "network" : facebookId, googleplusId, gamecenterId, gamecenter (no IDs to be supplied, logs in locally).
			- "id" : string containing the ID of the user. (respectivly facebook user ID, googleplus ID, gamecenter ID)
			- "secret" : string containing the secret associated with the user ID. (respectivly accesstoken, token, n/s)
			- "options" : used with network="gamecenter" to provide additional options for login (see LoginNetwork).
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"done" : 1
		*/
        void Link(const CotCHelpers::CHJSON* aConfiguration, CResultHandler *aHandler);
        DEPRECATED void Link(CResultHandler *aHandler, const CotCHelpers::CHJSON* aConfiguration);
		DEPRECATED void Link(const char *aNetwork, CResultHandler *aHandler);

		/**
			Method used to Unlink an account with Facebook, Googleplus, GameCenter.
			@param aNetwork is a string containing the network : "facebook", "googleplus", "gamecenter"
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"done" : 1
		*/
        void Unlink(const char *aNetwork, CResultHandler *aHandler);

		/**
			Can convert the current account to a Facebook, Googleplus, GameCenter or e-mail account. To be
			done only when being logged in with an anonymous account.
			@param aConfiguration is a JSON object holding the necessary details.
			The mandatory keys are:
			- "network": string containing the network: "facebook", "googleplus", "gamecenter" or "email".
			- "id": in case the network is "email", contains the e-mail to be used.
			- "secret": in case the network is "email", contains the password to be used.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"done" : 1
		 */
        void Convert(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);
        DEPRECATED void Convert(CResultHandler *aHandler, const CotCHelpers::CHJSON *aConfiguration);

		/** @}
		 * @defgroup user_profile Profile related tasks
		 * @{ */
		
		/**
			Method used to check if a user is registered with a network.
			@param  aIdent is the ID within the specified social network.
			@param  aNetwork the social network to look into.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"gamer_id" : <aident>
		*/
		void UserExist(const char *aIdent, const char *aNetwork, CResultHandler *aHandler);
		
		/** Method used to send an mail to a user registered with 'email' network  and who lost
            his/her paswword.
			@param  aJSON is a JSON object holding:
		 		- email : the user email
		 		- from : an email address to set the sender
		 		- title : the title of the mail to be sent
		 		- body : the body (text) of the email to be sent, the body should consain the tag
                [[SHORTCODE]] which will be replaced by the actual generated short code.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
		 	When the user receives the email, it will contain a short code. This short code needs to be
            used in the loginNetwork method with the network set to "restore" , the id to "" (empty) and
			the secret to the actual short code.
		 */
		void MailPassword(const CotCHelpers::CHJSON *aJSON, CResultHandler *aHandler);

		/** Once logged, the user registered with 'email' network  can change
		 his/her paswword.
			@param  aNewPassword the new password
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
		 	{
		 		"done" : 1
		 	}
		 */
		void ChangePassword(const char *aNewPassword, CResultHandler *aHandler);

		/** Once logged, the user registered with 'email' network  can change
		 his/her email.
			@param  aNewEmail the new email
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
		 {
		 "done" : 1
		 }
		 */
		void ChangeEmail(const char *aNewEmail, CResultHandler *aHandler);

		/**
			Method used to associate some optional data to the logged in profile in a JSON dictionary.
			You can fill fields with key "email", "displayName", "lang", "firstName", "lastName",
			"addr1", "addr2", "addr3" and "avatar". Other fields will be ignored. These fields must be
			strings, and some are pre-populated when the account is created, using the available info
			from the social network used to create the account.
			@param aJson is a JSON object holding the data to save for this user. The object can hold
			the whole profile or just a subset of the keys.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"done" : 1
		*/
		void SetProfile(const CotCHelpers::CHJSON *aJson, CResultHandler *aHandler);

		/**
			Method used to retrieve some optional data of the logged in profile previously set by
			method SetProfile.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			{
				"displayname" : "xxx",
				"email" : "xxx',
				"lang" : "en"
			}
		*/
		void GetProfile(CResultHandler *aHandler);
		
		/**
			Method to save a list of properties that will be used to find opponents. You can
			optionnally use it to save other informations which do not need to be used for your
			match making needs though.
			@param aPropertiesList is a JSON containing only basic types (bool, string,
			number. To save a date, use the UTC date in JSON string format (2013-05-17T10:43:59.599Z).
			i.e. { "level" : 10,  "board" : "square" }
			@param aDomain is the domain in which the properties have to be saved. "private" means they
            are valid only for this game.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
            @result if noErr, the json passed to the handler may contain:
			"properties" : {
				"level" : 10,
				"board" : "square",
			}
		*/
        void SetProperties(const CotCHelpers::CHJSON* aPropertiesList, const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void SetProperties(CResultHandler *aHandler, const CotCHelpers::CHJSON* aPropertiesList, const char *aDomain="private");
		
		/**
			Method to get the list of previously saved properties used to find opponents for a match.
            @param aDomain is the domain in which the properties have to be loaded. "private" means they
            are valid only for this game.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"properties" : {
				"level" : 10,
				"board" : "square",
			}
		*/
        void GetProperties(const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void GetProperties(CResultHandler *aHandler, const char *aDomain="private");
		
		
		/**
			Method to save  one property that will be used to find opponents. You can
			optionnally use it to save other informations which do not need to be used for your
			match making needs though.
			@param aProperty is a JSON containing a key and a value only basic types (bool, string,
			number. To save a date, use the UTC date in JSON string format (2013-05-17T10:43:59.599Z).
			i.e. { "key" : "level",  "value" : 10 }
            @param aDomain is the domain in which this property has to be saved. "private" means it is
            valid only for this game.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"properties" : {
		 		"level" : 10,
			}
		 */
        void SetProperty(const CotCHelpers::CHJSON* aProperty, const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void SetProperty(CResultHandler *aHandler, const CotCHelpers::CHJSON* aProperty, const char *aDomain="private");
		
		/**
			Method to get the list of previously saved properties used to find opponents for a match.
			@param aField the key to retreive
            @param aDomain is the domain in which this property has to be loaded. "private" means it is
            valid only for this game.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			"properties" : {
		 		"level" : 10,
			}
		 */
        void GetProperty(const char *aField, const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void GetProperty(CResultHandler *aHandler, const char *aField, const char *aDomain="private");

		/**
			Method to delete a single property of the global JSON object stored for this user and domain.
			@param aField the key to delete
            @param aDomain is the domain in which this property has to be deleted. "private" means it is
            valid only for this game.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
            @result if noErr and no binary key was passed in the configuration, the json passed to the handler may contain:
			{
                "done": 1
			}
		 */
        void DeleteProperty(const char *aField, const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void DeleteProperty(CResultHandler *aHandler, const char *aField, const char *aDomain="private");

		/** @}
		 * @defgroup user_transaction Transaction (key/value storage, also called "units") related tasks
		 * @{ */

		/**
			Method to retrieve the user's balance.
			@param aDomain the domains on which action must be taken. NULL is used for "private",
			which means it can only be accessed from this game.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			{
				"Gold" : 100,
				"Silver" : 10
			}
		*/
		void Balance(const char *aDomain, CResultHandler *aHandler);

		/**
			Method to retreive the history of transactions for this user.
			@param aDomain the domains on which action must be taken. NULL is used for "private",
			which means it can only be accessed from this game.
			@param aJSONOptions specify skip and limit and/or the currency for the history, if empty returns all the transactions.
				i.e. { "unit" : "Gold", "skip": 10, "limit" : 10}
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain: @code
			"servertime":	"2014-10-06T13:59:04.308Z", -- server actual time, to synchronize before any new transaction if needed
			"history":	[{
				"domain":	"com.clanofthecloud.cloudbuilder.xxxx",
				"ts":	"2014-10-06T05:05:38.764Z",
				"tx":	{
					"Gold":	-100,
					"Silver":	1
				},
				"desc":	"Unit Testing transactions, Zeroing Gold"
			}, {
				"domain":	"com.clanofthecloud.cloudbuilder.xxx",
				"ts":	"2014-10-06T05:05:38.749Z",
				"tx":	{
				"Gold":	100
				},
				"desc":	"Unit Testing transactions"
			}] -- ordered by descending time @endcode
		*/
		void TxHistory(const char *aDomain, const CotCHelpers::CHJSON *aJSONOptions, CResultHandler *aHandler);

		/**
			Method to read a single key of the global JSON object stored for this user and this domain.
			@param aConfiguration is a JSON configuration, that may contain
			- domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			- key: name of the key to retrieve from the global JSON object (if not passed, all keys are returned)
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr and no binary key was passed in the configuration, the json passed to the handler may contain:
			{
				"<key1>" : "value1", "<key2>" : "value2", ...
			}
		*/
		void KeyValueRead(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
			Method to insert a key or modify the global JSON object stored for this user and this domain.
			Please note that if you have used BinaryWrite with a key you reuse in KeyValueWrite, you will
			lose your initial binary data.
			@param aConfiguration JSON allowing for extensible configuration, that may contain:
			- domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			- key: name of the key to write to (if not passed, the JSON itself is replaced)
			- data: a JSON containing the object to write
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr, the json passed to the handler may contain:
			{
				"<key1>" : "value1", "<key2>" : "value2", ...
			}
		 */
		void KeyValueWrite(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
			Method to delete a single key of the global JSON object stored for this user and domain.
			@param aConfiguration is a JSON configuration, that may contain
			- domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			- key: name of the key to delete from the global JSON object. BEWARE: if you omit the key, ALL entries
            will be deleted!
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if noErr and no binary key was passed in the configuration, the json passed to the handler may contain:
			{
		 		"done": 1
			}
		 */
		void KeyValueDelete(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
			 Method to read a single key containing binary data stored for this user and this domain.
			 @param aConfiguration JSON allowing for extensible configuration, that may contain:
			 - domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			 - key: name of the key to retrieve
			 @param aHandler result handler whenever the call finishes (it might also be synchronous)
			 @result if noErr, the json passed to the handler may contain:
			 {
			 "url" : "<signed URL>",
			 }
			 CCloudResult.HasBinary() must be true and you can acces to the data through :
		 */
		void BinaryRead(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);
		
		/**
			 Method to insert or modify a single key represented by binary data
			 Please note that if you have used KeyValueWrite with a key you reuse in BinaryWrite, you will
			 lose your initial JSON data.
			 @param aConfiguration JSON allowing for extensible configuration, that may contain:
			 - domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			 - key: name of the key to write to
			 @param aPointer is the binary array you want to save.
			 @param aSize is the size of the binary array, in bytes.
			 @param aHandler result handler whenever the call finishes (it might also be synchronous)
			 @result if noErr, the json passed to the handler may contain:
			 {
			 "url" : "<signed URL>",
			 }
		 */
		void BinaryWrite(const CotCHelpers::CHJSON *aConfiguration, const void *aPointer, size_t aSize, CResultHandler *aHandler);

		/**
		 Method to remove data pointed by a single key.
		 @param aConfiguration JSON allowing for extensible configuration, that may contain:
		 - domain: the domain on which the action is to be taken (if not passed, the private domain is used)
		 - key: name of the key to delete
		 @param aHandler result handler whenever the call finishes (it might also be synchronous)
		 @result if noErr, the json passed to the handler may contain:
		 {
		 "done" : 1,
		 }
		 */
		void BinaryDelete(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/** @}
		 * @defgroup user_event Event related tasks
		 * @{ */

		/**
			Method to be called to setup a listener on a domain.
			@param aDomain is the domain (ie channel) to watch
			@param aListener will be called when an event will be available on this domain. The listener
			is a CRefClass, so if you don't need it, you can call Release(). The system will keep it as
			long as needed and free it then, as shown below.
			CEventListener *listener = new MyListenerObject;
			RegisterEventListener("domain", listener);
			listener->Release();
			Adversely, do not delete the listener object once registered. Unregister it before or call
			Release(), which will do nothing if it is still retained elsewhere.
		*/
		void RegisterEventListener(const char *aDomain, CEventListener *aListener);
		
		/**
			Unregisters a previously registered event listener.
			Does nothing if the event listener wasn't actually registered for the matching domain.
			@param aDomain is the domain (ie channel) to watch
			@param aListener a previously registered event listener
		 */
		void UnregisterEventListener(const char *aDomain, CEventListener *aListener);
		
		/**
			Method to be called to setup a listener on a domain.
			@param aDomain is the domain (ie channel) to watch
			@param aGamerID the user to notify
			@param aEvent the data to transmit (free form json)
			@param aNotification  an optionnal json which describe the OS notification to be displayed
			is the app is in background
			It should be of the form { "en" : "I need help!", "fr" : "J'ai desoin d'aide"}
			the size of the localized message should be less than 100 char and will be truncated.
			the lang code is ISO-639-1. Apply Cloudbuilder::CUserManager::SetProfile with 'lang',
			preferred language, should be set with this ISO code.If 'lang' is no set, or the code not
			found in the aNotification, then, the 'en' will de displayed. If even 'en' is not found,
			a cloudbuilder default message will be displayed. If NULL is passed, no notification will
			be displayed.
			aNotification can optionnaly contains a "data" field that will be part of the os
			notification, but the size of this field plus the localized message should be less than
			100 character (this is due to os constraint).
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
		*/
		void PushEvent(const char *aDomain, const char* aGamerID, const CotCHelpers::CHJSON *aEvent, const CotCHelpers::CHJSON *aNotification, CResultHandler *aHandler);
		
		/** @} */

		/**
			Method used to retrieve the displayName given by the user at creation time or by a call to SetProfile.
			@result is a C string holding the pseudo value.
		*/
		const char *GetDisplayName();

		/**
			Method used to retrieve the GamerID.
			@result is a C string holding the gamer ID value.
		*/
		const char *GetGamerID();
		
		/**
			Method used to retrieve the email address given by the user at creation time or by a call
			to SetProfile .
			@result is a C string holding the email address value.
		*/
		const char *GetMail();

		/**
		 	Obsolete method.
		*/
		DEPRECATED void Publish(const char *aNetwork, const CotCHelpers::CHJSON* aMessage, CResultHandler *aHandler);

		/**
			Method to call in order to generate a temporary code that can be used to obtain a new godchild.
            @param aDomain is the domain in which the godfather link should be established. "private" means
            it's local to this game only.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if enNoErr is sent back to your handler, the "result" of the embedded JSON will have a
			value which may contain:
			{ "godfathercode" : "xxx" }
		*/
        void GetGodfatherCode(const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void GetGodfatherCode(CResultHandler *aHandler, const char *aDomain="private");
		
		/**
			Method used to retrieve the godfather of the currently logged in user.
            @param aDomain is the domain in which the godfather link should be established. "private" means
            it's local to this game only.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if enNoErr is sent back to your handler, the "result" of the embedded JSON will have a
			value which may contain:
			{ "godfather" : {
				"gamer_id" : "xxx",
				"profile" : {
					CotC Profile JSON
				}
			}
		*/
        void GetGodfather(const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void GetGodfather(CResultHandler *aHandler, const char *aDomain="private");
		
		/**
			Method to call to attribute a godfather to the currently logged in user.
			@param aCode is a string as generated by @CUser::GetGodfatherCode.
			@param aOptions is a JSON object containing optionnal information. The JSON object may contains
			the following keys:
			'maturity'  : integer, age in number of days of the godchildren. (since the register date), if maturity is 0 or missing,
		 	no checks are made.
		 	'domain' : "private" or the wanted domain
		 	'reward' : a transaction Json rewarding the godfather formed as
		 		{ 	transaction : { "unit" : amount},
		 			description : "reward transaction",
		 			domain : "com.clanoftcloud.text.DOMAIN" }
		 		where desciption and domain are optionnals
		 	'osn' :  {"en" : "You've been rewarded"} 
		 		if you want an OS notification for the godfather app
             NB : the godfather will reveive an event of type 'godchildren' containing the id of the godchildren and the balace/achievements field if rewarded
		 
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if enNoErr is sent back to your handler, the "result" of the embedded JSON may contain:
			{ 	
		 		"done" : "1",
		 	}
		*/
		void SetGodfather(const char *aCode, const CotCHelpers::CHJSON *aOptions, CResultHandler *aHandler);
		
		/**
			Method used to retrieve all the godchildren for the currently logged in user.
            @param aDomain is the domain in which the godfather link should be established. "private" means
            it's local to this game only.
            @param aHandler result handler whenever the call finishes (it might also be synchronous)
			@result if enNoErr is sent back to your handler, the "result" of the embedded JSON will have a
			value which may contain:
			{ "godchildren" :
				[ { "gamer_id" : "xxx", "profile" :CotC Profile JSON },
				  .....
				]
			}
		*/
        void GetGodchildren(const char *aDomain, CResultHandler *aHandler);
        DEPRECATED void GetGodchildren(CResultHandler *aHandler, const char *aDomain="private");

		/** Method used to retrieve global user information, including profile, friends, devices, ...
			It's the same JSON as the one received upon the Login/ResumeSession process.
			@param aHandler result handler whenever the call finishes (it might also be synchronous)
		*/
		void Outline(CResultHandler *aHandler);
		
		/**
			Method used to signal that the currently logged in user is ready to receive push notifications.
			Usually, this is called internally by CotC at login time, unless CClan::Setup has been called
			with an option to delay the registration.
		*/
		void RegisterForNotification();

		/** @} @defgroup user_achievements Achievement related tasks @{ */

		/**
		 * Fetches information about the status of the achievements configured for this game.
		 * Additional configuration can be provided in the form of JSON data.
		 * @param aConfiguration JSON data
			- domain: the domain on which to list achievements (if none, uses the private one)
		 * @param aHandler result handler, which will receiving the following data:
		 * @result if noErr, the json passed to the handler may contain the following: @code
			"achievements": {
				"achievementName": {
					"type": "limit",
					"config":       {
						"maxValue": 10,
						"maxTriggerCount": -1,
						"unit": "b",
						"rewardTx": {
							"b": "-auto",
							"a": 10
						}
					},
					"progress": 0
				}
			} @endcode
			So you may use the following code to check the progress of a given achievement: @code
			void ResultHandler(eErrorCode code, const CCloudResult *result) {
				float progress = result->GetJSON()->GetSafe("achievements")->GetSafe("achievementName")->GetFloat("progress");
			}
			@endcode
		 */
		void ListAchievements(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
		 * Helper method that allows easily to post a success. For this to work, you need to have an achievement
		 * that uses the same unit as the name of the achievement.
		 * @param aUnit the name of the achievement, and unit to increment
		 * @param aIncrement by how much to increment the unit; you may as well decrement it by passing a negative value
		 * @param aGamerData gamer data to store for the gamer/achievement, merged with the current data (that is,
		 * existing keys are not affected)
		 * @param aHandler handler called with the result of a call to CUserManager::Transaction
		 */
		void EarnAchievement(const char *aUnit, int aIncrement = 1, const CotCHelpers::CHJSON *aGamerData = NULL, CResultHandler *aHandler = NULL);

		/**
		 * Method to send a transaction for this user.
		 * @param aConfiguration the JSON describing the configuration, which may include:
			 - domain: the domain on which the action is to be taken (if not passed, the private domain is used)
			 - transaction: a json with the transaction as in CUserManager::Transaction
			 - description: a string describing the transaction
		 * @param aHandler the handler called with the result when the call completes
		 * @result if noErr, a JSON which may contain the following: @code
			"balance": {
				"Gold" : 100,
				"Silver" : 10
			},
			"achievements": [
				{
					"name": "test",
					"type": "limit",
					"config": {
						"type": "limit",
						"maxValue": 1000,
						"unit": "score"
					}
				}
			]
			} @endcode
		 */
		void TransactionExtended(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
		 * Allows to store arbitrary data for a given achievement and the current player (appears in the 'gamerData' node of achievements).
		 * @param aConfiguration JSON data describing the operation to perform
			- domain: domain identifying the achievement 
			- name: name of the achievement
			- data: data to set (freeform)
		 * @param aHandler the handler called with the result when the call completes
		 * @result if noErr, the json passed to the handler will contain the updated definition of the achievement
		 */
		void SetAchievementData(const CotCHelpers::CHJSON *aConfiguration, CResultHandler *aHandler);

		/**
		 * Run a user authenticated batch on the server side.
		 * Batch is edited on BackOffice server.
		 * @param aConfiguration JSON data describing the dat to pass to the batch
		 - domain: the domain on which the action is to be taken (if not passed, the private domain is used)
		 - name: the name identifying the batch
		 * @param aParameters JSON data describing the data to pass to the batch
			- input: user defined data
		 * @param aHandler the handler called with the result when the call completes
		 * @result if noErr, the json passed to the handler will contain the result of the executed batch
		 */
        void Batch(const CotCHelpers::CHJSON *aConfiguration, const CotCHelpers::CHJSON *aParameters, CResultHandler *aHandler);
        DEPRECATED void Batch(CResultHandler *aHandler, const CotCHelpers::CHJSON *aConfiguration, const CotCHelpers::CHJSON *aParameters);

		/** @} */

private:
		struct PersistedLoginParams {
			CotCHelpers::cstring mGamerId, mGamerSecret;
		};
		static CUserManager *mInstance;
		// Call ReadLoginParamsFromDisk prior to use, and PersistLoginParams after finishing modify these.
		PersistedLoginParams persistedLoginParams;
		CGloballyKeptHandler<CResultHandler> &loginDoneHandler, &linkDoneHandler, &convertDoneHandler;
		CGloballyKeptHandler<CResultHandler> &binaryDoneHandler;
		CotCHelpers::cstring mDeviceOS, mDeviceToken, mAccountNetwork;
		
		// Not intended to be overloaded
		CUserManager();
		~CUserManager();

		// Use these in combinationn with access to persistedLoginParams
		void CommitLoginParams();
		void ReadLoginParams(PersistedLoginParams *dest);

		/**
		 * Call this upon Setup on the CClan.
		 * @param handler handler to call once finished (should always return enNoErr)
		 */
		void DoSetupProcesses(CResultHandler *handler);
		
		
		void LinkWithOther(const char *networkToExclude);
		void linkDone(const CCloudResult *res);
		void LoginAnonymousDone(CCloudResult *res);
		void LoginDone(CCloudResult *result);
		void convertDone(const CCloudResult *res);
		void LogoutDone(CCloudResult *result);
		
		void LogWithGC(const CCloudResult *result, const CotCHelpers::CHJSON *aOptions);
		eErrorCode LinkWithGC(eErrorCode ec, const char *gcid, const char *displayname);
		void LogWithGCDone(const CCloudResult *result);
		void ConvertToGC(CCloudResult *result);
	
		bool IsAnonymousAccount();

		void binaryWriteDone(const CCloudResult *result, const void *, size_t, CResultHandler *);
		void binaryUploadDone(const CCloudResult *result, char*, CResultHandler *aHandler);
		void binaryReadDone(const CCloudResult *result, CResultHandler *aHandler);

		void didLogin(const CCloudResult *result);

		/**
		 * You need to call this in CClan::Terminate().
		 */
		void Terminate();

		friend class CClan;
		friend struct singleton_holder<CUserManager>;
		friend void ::LaunchAuthenticate(void);
		friend void publishUserAfterPermission(int err, void *params);

		void RegisterDevice(const char *aOS, const char *aToken);
		friend void AchieveRegisterDevice(unsigned long len, const void *bytes);
		friend FACTORY_FCT void CUserManager_ResetAchievements(const char *domain, CResultHandler *handler);
	};
	
}


#endif
