// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ULobbyMember.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/isteammatchmaking.h"
#include "USteamLobby.generated.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

/**
 * 
 */
UCLASS()
class STEAMWORKSDEMO_API USteamLobby : public UObject
{
	GENERATED_BODY()

public:
	/**
	* SteamLobby Initializer
	* 
	* Note: You still need to call Initialize before
	* the lobby will be considered valid.
	*/
	USteamLobby(const FObjectInitializer& ObjectInitializer);

	CSteamID GetLobbyID();

	/**
	* Sets the lobby ID and marks this lobby instance
	* as valid.
	*/
	void Initialize(const CSteamID& InLobbyId);

	/**
	* Builds and executes a new LoadLevel message to the lobby
	*/
	void SendLoadLevelMessage(const FString& LevelName);
	
	/**
	* Returns true if this is a valid and active steam lobby.
	* 
	* Note: This ID could still technically fail if the player
	* were to lose connection or the lobby was disbanded
	* unexpectedly.
	*/
	UFUNCTION(BlueprintCallable, Category="Steamworks")
	bool IsActive() const;

	TArray<ULobbyMember*> GetMembers();

	void UpdateAllLobbyMemberData();
	void GetAllLobbyMembersData();
	TArray<CSteamID> GetAllLobbyMemberID();

private:
	CSteamID LobbyID;
};
