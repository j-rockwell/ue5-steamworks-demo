// Fill out your copyright notice in the Description page of Project Settings.

#include "USteamGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/isteammatchmaking.h"
#include "USteamLobby.h"
#include "UPlayerAvatarWidget.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

USteamGameInstance* USteamGameInstance::Instance = nullptr;
USteamLobby* LobbyInstance = nullptr;
UPlayerAvatarWidget* AvatarWidget = nullptr;

/* Working example to create and set a new avatar widget to the screen
	// Queue the class loading to the game thread to ensure thread safety
		AsyncTask(ENamedThreads::GameThread, [this]() {
			FStringClassReference MyWidgetClassRef(TEXT("WidgetBlueprint'/Game/UI/WBP_TestWidget.WBP_TestWidget_C'")); // Ensure the path is correct
			UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>();

			if (!MyWidgetClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load MyWidgetClass"));
				return;
			}

			// Proceed to create and display the widget, now safely on the game thread
			UWorld* CurrentWorld = GetWorld();
			if (!IsValid(CurrentWorld)) {
				UE_LOG(LogTemp, Warning, TEXT("CurrentWorld is null"));
				return;
			}

			UPlayerAvatarWidget* PlayerAvatarWidget = CreateWidget<UPlayerAvatarWidget>(CurrentWorld, MyWidgetClass);
			if (PlayerAvatarWidget)
			{
				if (IsValid(PlayerAvatarWidget->PlayerAvatarText))
				{
					UE_LOG(LogTemp, Log, TEXT("PlayerAvatarText found at call time"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("PlayerAvatarText nullptr at call time"));
				}

				const char* Username = SteamFriends()->GetPersonaName();
				PlayerAvatarWidget->AddToViewport();
				PlayerAvatarWidget->SetPlayerAvatarText(Username);
				UE_LOG(LogTemp, Log, TEXT("SetPlayerAvatarText called"));
			}
		});
*/

USteamGameInstance::USteamGameInstance()
{
	Instance = this;
}

USteamGameInstance* USteamGameInstance::Get()
{
	return Instance;
}

USteamLobby* USteamGameInstance::GetLobby()
{
	return LobbyInstance;
}

void USteamGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("SteamManager Init"));
	
	if (!SteamAPI_Init())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to initialize steam"));
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize Steam"));
		return;
	}

	bIsSteamReady = true;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Steam initialized"));
	UE_LOG(LogTemp, Log, TEXT("Steam initialized"));

	GetWorld()->GetTimerManager().SetTimer(MessageCheckTimerHandle, this, &USteamGameInstance::CheckInboundMessages, 1.0f, true);
}

void USteamGameInstance::Shutdown()
{
	GetWorld()->GetTimerManager().ClearTimer(MessageCheckTimerHandle);
	SteamAPI_Shutdown();
	Super::Shutdown();
}

void USteamGameInstance::CreateLobby()
{
	if (!bIsSteamReady)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attempted to create a lobby when Steam was not ready"));
		UE_LOG(LogTemp, Error, TEXT("Attempted to create a lobby when Steam was not ready"));
		return;
	}

	if (LobbyInstance && LobbyInstance->IsActive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attempted to create a lobby when one already exists"));
		UE_LOG(LogTemp, Error, TEXT("You are already in an active lobby"));
		return;
	}

	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 4);
	m_OnLobbyCreatedResult.Set(hSteamAPICall, this, &USteamGameInstance::OnLobbyCreated);
}

void USteamGameInstance::DebugLobby()
{
	if (!bIsSteamReady)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attempted to debug a lobby when Steam was not ready"));
		UE_LOG(LogTemp, Error, TEXT("Attempted to debug a lobby when Steam was not ready"));
		return;
	}

	if (!LobbyInstance || !LobbyInstance->IsActive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attempted to debug a lobby that does not exist"));
		UE_LOG(LogTemp, Error, TEXT("You are not in a lobby"));
		return;
	}

	const char* TestName = SteamFriends()->GetPersonaName();
	UE_LOG(LogTemp, Display, TEXT("Found Name using GetPersonaName directly: %s"), FString(TestName));

	TArray<ULobbyMember*> members = LobbyInstance->GetMembers();
	for (ULobbyMember* member : members)
	{
		UE_LOG(LogTemp, Display, TEXT("Found Member: %s"), FString(member->GetName()));
	}
}

void USteamGameInstance::OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback)
{
	UE_LOG(LogTemp, Log, TEXT("OnLobbyChatUpdate called"));
	if (pCallback->m_rgfChatMemberStateChange & k_EChatMemberStateChangeEntered)
	{
		CSteamID PlayerID = pCallback->m_ulSteamIDUserChanged;
		int32 Position = SteamMatchmaking()->GetNumLobbyMembers(LobbyInstance->GetLobbyID()) - 1;
		
		if (AvatarWidget)
		{
			AsyncTask(ENamedThreads::GameThread, [this, Position, PlayerID]() {
				const char* Username = SteamFriends()->GetFriendPersonaName(PlayerID);
				AvatarWidget->SetPlayerAvatar(Position, Username, GetSteamAvatarTexture(PlayerID));
				UE_LOG(LogTemp, Log, TEXT("Joining User: %s"), FString(UTF8_TO_TCHAR(Username)));
			});
		}
	}
}

void USteamGameInstance::OnLobbyCreated(LobbyCreated_t* pResult, bool bIOFailure)
{
	if (bIOFailure || pResult->m_eResult != k_EResultOK)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create Steam lobby"));
		return;
	}

	CSteamID LobbySteamID = pResult->m_ulSteamIDLobby;
	SteamMatchmaking()->SetLobbyData(LobbySteamID, "name", "Steamworks Demo Lobby");

	USteamLobby* NewLobby = NewObject<USteamLobby>(this, USteamLobby::StaticClass());
	if (NewLobby)
	{
		NewLobby->Initialize(LobbySteamID);
		LobbyInstance = NewLobby;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Steamworks Demo Lobby Created"));

		AsyncTask(ENamedThreads::GameThread, [this]() {
			FStringClassReference MyWidgetClassRef(TEXT("WidgetBlueprint'/Game/UI/WBP_TestWidget.WBP_TestWidget_C'")); // Ensure the path is correct
			UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>();

			if (!MyWidgetClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load MyWidgetClass"));
				return;
			}

			// Proceed to create and display the widget, now safely on the game thread
			UWorld* CurrentWorld = GetWorld();
			if (!IsValid(CurrentWorld)) {
				UE_LOG(LogTemp, Warning, TEXT("CurrentWorld is null"));
				return;
			}

			AvatarWidget = CreateWidget<UPlayerAvatarWidget>(CurrentWorld, MyWidgetClass);
			if (AvatarWidget)
			{
				const char* Username = SteamFriends()->GetPersonaName();
				AvatarWidget->AddToViewport();
				AvatarWidget->SetPlayerAvatar(0, Username, GetSteamAvatarTexture(LobbyInstance->GetMembers()[0]->GetSteamID()));
				UE_LOG(LogTemp, Log, TEXT("SetPlayerAvatar called"));
			}
		});
	}
}

void USteamGameInstance::CheckInboundMessages()
{
	uint32 Size;
	while (SteamNetworking()->IsP2PPacketAvailable(&Size))
	{
		TArray<uint8> ReceivedData;
		ReceivedData.SetNumUninitialized(Size);

		uint32 BytesRead = 0;
		CSteamID SenderSteamID;
		if (SteamNetworking()->ReadP2PPacket(ReceivedData.GetData(), Size, &BytesRead, &SenderSteamID))
		{
			FString Message = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
			HandleLevelChangeMessage(Message);
		}
	}
}

void USteamGameInstance::NotifyLevelChange(const FString& LevelName)
{
	FString Command = FString(TEXT("LoadNewLevel:")) + LevelName;
	TArray<CSteamID> LobbyMembers = LobbyInstance->GetAllLobbyMemberID();
	for (const CSteamID& MemberSteamID : LobbyMembers)
	{
		SendP2PMessageToMember(MemberSteamID, Command);
	}
}

void USteamGameInstance::HandleLevelChangeMessage(const FString& Message)
{
	if (Message.StartsWith(TEXT("LoadNewLevel:")))
	{
		FString LevelName = Message.Mid(13); // Skip "LoadNewLevel:"
		LoadLevel(LevelName);
	}
}

void USteamGameInstance::LoadLevel(const FString& LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::OpenLevel(World, FName(*LevelName));
	}
}

bool USteamGameInstance::SendP2PMessageToMember(const CSteamID& RecipientSteamID, const FString& Message)
{
	if (Message.IsEmpty())
		return false;

	// Convert FString to an array of bytes
	TArray<uint8> MessageData;
	MessageData.AddUninitialized(Message.Len() + 1);
	StringCbCopyA((char*)MessageData.GetData(), MessageData.Num(), TCHAR_TO_ANSI(*Message));

	// Send a P2P packet
	return SteamNetworking()->SendP2PPacket(RecipientSteamID, MessageData.GetData(), MessageData.Num(), k_EP2PSendReliable);
}

UTexture2D* USteamGameInstance::GetSteamAvatarTexture(CSteamID MemberID)
{
	if (!SteamAPI_Init()) return nullptr;

	// Request the player's avatar
	int Image = SteamFriends()->GetLargeFriendAvatar(MemberID);
	if (Image == -1) return nullptr; // Image not available or still loading

	uint32 Width, Height;
	if (!SteamUtils()->GetImageSize(Image, &Width, &Height)) return nullptr;

	// Create texture
	UTexture2D* PlayerAvatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
	if (!PlayerAvatar) return nullptr;

	// Lock the texture for editing
	void* MipData = PlayerAvatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

	// Copy the Steam avatar into the texture
	SteamUtils()->GetImageRGBA(Image, (uint8*)MipData, 4 * Width * Height);

	// Unlock the texture and update
	PlayerAvatar->PlatformData->Mips[0].BulkData.Unlock();
	PlayerAvatar->UpdateResource();

	return PlayerAvatar;
}

