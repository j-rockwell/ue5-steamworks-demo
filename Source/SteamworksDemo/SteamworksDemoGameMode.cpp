// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteamworksDemoGameMode.h"
#include "SteamworksDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASteamworksDemoGameMode::ASteamworksDemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
