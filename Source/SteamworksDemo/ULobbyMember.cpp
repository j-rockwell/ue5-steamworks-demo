// Fill out your copyright notice in the Description page of Project Settings.


#include "ULobbyMember.h"

CSteamID ULobbyMember::GetSteamID()
{
	return ID;
}

const char* ULobbyMember::GetName()
{
	return Name;
}

FString ULobbyMember::GetDisplayName()
{
	return FString(UTF8_TO_TCHAR(Name));
}

UTexture2D* ULobbyMember::GetAvatar()
{
	return Avatar;
}

void ULobbyMember::Load(CSteamID SteamID)
{
	const char* PersonaName = SteamFriends()->GetFriendPersonaName(SteamID);
	if (!PersonaName)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Persona Name from Steam API"));
		return;
	}

	ID = SteamID;
	Name = PersonaName;
	Avatar = GetAvatarTexture();
}

UTexture2D* ULobbyMember::GetAvatarTexture()
{
	int32 AvatarId = SteamFriends()->GetLargeFriendAvatar(ID);
	if (AvatarId == -1) return nullptr;

	uint32 Width, Height;
	if (SteamUtils()->GetImageSize(AvatarId, &Width, &Height))
	{
		TArray<uint8> AvatarRGBA;
		AvatarRGBA.SetNum(Width * Height * 4);

		if (SteamUtils()->GetImageRGBA(AvatarId, AvatarRGBA.GetData(), AvatarRGBA.Num()))
		{
			UTexture2D* AvatarTexture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);;
			if (AvatarTexture)
			{
				void* TextureData = AvatarTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, AvatarRGBA.GetData(), AvatarRGBA.Num());
				AvatarTexture->PlatformData->Mips[0].BulkData.Unlock();
			}

			return AvatarTexture;
		}
	}

	return nullptr;
}
