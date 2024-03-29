// Fill out your copyright notice in the Description page of Project Settings.

#include "UPlayerAvatarWidget.h"
#include "UMG.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"

#pragma comment(lib, "ThirdParty/Steamworks/Steamv153/sdk/redistributable_bin/win64/steam_api64.lib")

void UPlayerAvatarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Clear previous bindings if any
    PlayerAvatarTextBlocks.Empty();
    PlayerAvatarImages.Empty();

    // Loop through the set number of player slots
    for (int32 i = 0; i <= 3; ++i)
    {
        // Construct the widget names based on the current index
        FName TextBlockName = FName(*FString::Printf(TEXT("PlayerAvatarText_%d"), i));
        FName ImageName = FName(*FString::Printf(TEXT("PlayerAvatarImage_%d"), i));

        // Try to findand bind the TextBlock widget
        if (UTextBlock* TextBlock = Cast<UTextBlock>(GetWidgetFromName(TextBlockName)))
        {
            PlayerAvatarTextBlocks.Add(TextBlock);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerAvatarText not found"));
            // Log or handle the case where the TextBlock was not found
        }

        // Try to find and bind the Image widget
        if (UImage* Image = Cast<UImage>(GetWidgetFromName(ImageName)))
        {
            PlayerAvatarImages.Add(Image);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerAvatarImage not found"));
        }
    }
}

void UPlayerAvatarWidget::SetPlayerAvatar(int32 Index, const FString& Name, UTexture2D* Avatar)
{
    if (Index < 0 || Index >= PlayerAvatarTextBlocks.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Index out of bounds, found %i blocks"), PlayerAvatarTextBlocks.Num());
        return;
    }
    
    UTextBlock* TargetTextBlock = PlayerAvatarTextBlocks[Index];
    UImage* TargetImage = PlayerAvatarImages[Index];

    if (!TargetTextBlock)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find target Text Block"));
        return;
    }

    if (!TargetImage)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find target Image Block"));
        return;
    }

    TargetTextBlock->SetOpacity(1.0f);
    TargetTextBlock->SetText(FText::FromString(Name));

    TargetImage->SetOpacity(1.0f);
    TargetImage->SetBrushFromTexture(Avatar, false);
}