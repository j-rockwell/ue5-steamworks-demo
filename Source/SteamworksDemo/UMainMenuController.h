// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class STEAMWORKSDEMO_API UMainMenuController : public UObject
{
 GENERATED_BODY()
 
public:
	/**
	* Lobby_OnPress is called when the 'Create Lobby'
	* button is pressed.
	*/
	 UFUNCTION(BlueprintCallable, Category="Steamworks")
	 static void Lobby_OnPress();

	 /**
	 * Debug_OnPress is called when the 'Debug'
	 * button is pressed.
	 */
	 UFUNCTION(BlueprintCallable, Category="Steamworks")
	 static void Debug_OnPress();

	 /**
	 * LoadLevel_OnPress is called when the 'Load Demo Level'
	 * button is pressed in the UI.
	 */
	 UFUNCTION(BlueprintCallable, Category="Steamworks")
	 static void LoadLevel_OnPress();
};