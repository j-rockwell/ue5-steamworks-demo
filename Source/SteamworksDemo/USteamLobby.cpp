// Fill out your copyright notice in the Description page of Project Settings.

#include "USteamLobby.h"
#include "USteamGameInstance.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/isteammatchmaking.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steamclientpublic.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

USteamLobby::USteamLobby(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Note: LobbyId is uninitialized until Initialize() is called
}

void USteamLobby::Initialize(const CSteamID& InLobbyId)
{
    LobbyID = InLobbyId;
}

bool USteamLobby::IsActive() const
{
    return LobbyID.IsValid();
}

CSteamID USteamLobby::GetLobbyID()
{
    return LobbyID;
}

TArray<CSteamID> USteamLobby::GetAllLobbyMemberID()
{
    TArray<CSteamID> result;
    int count = SteamMatchmaking()->GetNumLobbyMembers(LobbyID);
    for (int i = 0; i < count; i++)
    {
        CSteamID memberId = SteamMatchmaking()->GetLobbyMemberByIndex(LobbyID, i);
        result.Add(memberId);
    }

    return result;
}

TArray<ULobbyMember*> USteamLobby::GetMembers()
{
    CSteamID selfID = SteamUser()->GetSteamID();
    TArray<ULobbyMember*> result;

    int count = SteamMatchmaking()->GetNumLobbyMembers(LobbyID);
    for (int i = 0; i < count; i++)
    {
        CSteamID memberId = SteamMatchmaking()->GetLobbyMemberByIndex(LobbyID, i);
        ULobbyMember* member = NewObject<ULobbyMember>(this, ULobbyMember::StaticClass());
        member->Load(memberId);
        result.Add(member);
    }

    return result;
}

void USteamLobby::SendLoadLevelMessage(const FString& LevelName)
{
    FString Command = FString::Printf(TEXT("LoadNewLevel:%s"), *LevelName);

    // Assuming you have a method to retrieve the lobby members' SteamIDs
    TArray<CSteamID> LobbyMembers = GetAllLobbyMemberID();
    for (const CSteamID& MemberSteamID : LobbyMembers)
    {
        USteamGameInstance* SteamManager = USteamGameInstance::Get();
        if (!SteamManager->SendP2PMessageToMember(MemberSteamID, Command))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to send level load message to user %lld"), MemberSteamID.ConvertToUint64());
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("Sent level load message to user"));
    }
}