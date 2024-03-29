// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "ULobbyMember.generated.h"

/**
 * 
 */
UCLASS()
class STEAMWORKSDEMO_API ULobbyMember : public UObject
{
	GENERATED_BODY()

public:
	void Load(CSteamID steamID);

	CSteamID GetSteamID();
	const char* GetName();
	FString GetDisplayName();
	UTexture2D* GetAvatar();

private:
	CSteamID ID;
	const char* Name;
	UTexture2D* Avatar;

	UTexture2D* GetAvatarTexture();
};
