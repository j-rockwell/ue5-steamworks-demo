// Fill out your copyright notice in the Description page of Project Settings.

#include "UMainMenuController.h"
#include "USteamGameInstance.h"

void UMainMenuController::Lobby_OnPress()
{
	UE_LOG(LogTemp, Display, TEXT("Lobby_OnPress Triggered"));

	USteamGameInstance* SteamManager = USteamGameInstance::Get();
	if (SteamManager)
	{
		SteamManager->CreateLobby();
	}
}

void UMainMenuController::Debug_OnPress()
{
	UE_LOG(LogTemp, Display, TEXT("Debug_OnPress Triggered"));

	USteamGameInstance* SteamManager = USteamGameInstance::Get();
	if (SteamManager)
	{
		SteamManager->DebugLobby();
	}
}

void UMainMenuController::LoadLevel_OnPress()
{
	UE_LOG(LogTemp, Display, TEXT("LoadLevel_OnPress Triggered"));
	
	USteamGameInstance* SteamManager = USteamGameInstance::Get();
	if (SteamManager)
	{
		FString LevelName = TEXT("OpenWorld");
		SteamManager->LoadLevel(LevelName);
	}
}