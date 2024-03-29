// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameInstance.h"

void UTestGameInstance::Init()
{
	UE_LOG(LogTemp, Log, TEXT("Init called"));
}

void UTestGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("Shutdown called"));
}
