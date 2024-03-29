// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include <Components/TextBlock.h>
#include "UMG.h"
#include "UPlayerAvatarWidget.generated.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

/**
 * 
 */
UCLASS()
class STEAMWORKSDEMO_API UPlayerAvatarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Steamworks")
	void SetPlayerAvatar(int32 Index, const FString& Name, UTexture2D* Avatar);

protected:
	virtual void NativeConstruct() override;

	TArray<UTextBlock*> PlayerAvatarTextBlocks;
	TArray<UImage*> PlayerAvatarImages;
};
