// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USteamLobby.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/isteammatchmaking.h"
#include "USteamGameInstance.generated.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

UCLASS()
class STEAMWORKSDEMO_API USteamGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USteamGameInstance();

	/**
	* Return a static reference
	* to the current Steam Game Instance
	*/
	static USteamGameInstance* Get();

	USteamLobby* GetLobby();
	
	/**
	* Called when the Game Instance begins
	* 
	* This objects lifecycle starts when the application
	* opens and does not end unless it is closed or the
	* Game Instance is changed.
	*/
	virtual void Init() override;
	virtual void Shutdown() override;
	
	/**
	* Call to begin lobby creation. This function
	* will modify the return value of m_OnLobbyCreatedResult
	* which will then call OnLobbyCreated with the result.
	* 
	* If you need something performed once the lobby is created
	* use that function instead.
	*/
	void CreateLobby();

	/**
	* Call to begin debug output.
	*/
	void DebugLobby();

	void CheckInboundMessages();
	void NotifyLevelChange(const FString& LevelName);
	void HandleLevelChangeMessage(const FString& Message);
	void LoadLevel(const FString& LevelName);
	bool SendP2PMessageToMember(const CSteamID& RecipientSteamID, const FString& Message);

	UTexture2D* GetSteamAvatarTexture(CSteamID MemberID);

private:
	static USteamGameInstance* Instance;

	bool bIsSteamReady = false;

	// Lobby creation
	/**
	* Lobby creation callback result
	*/
	CCallResult<USteamGameInstance, LobbyCreated_t> m_OnLobbyCreatedResult;

	// Steam callback for lobby chat updates
	STEAM_CALLBACK(USteamGameInstance, OnLobbyChatUpdate, LobbyChatUpdate_t);

	/**
	* OnLobbyCreated is called when the Lobby Created result
	* is returned. This can occur 1-5 seconds after CreateLobby
	* has been called.
	*/
	void OnLobbyCreated(LobbyCreated_t* pResult, bool bIOFailure);

	FTimerHandle MessageCheckTimerHandle;
};
