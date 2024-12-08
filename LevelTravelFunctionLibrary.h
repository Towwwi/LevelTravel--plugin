// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "LevelTravelFunctionLibrary.generated.h"

#define CHECK_GAMEPLAY_TAG_VALIDITY(GameplayTag) \
if (!UGameplayTagsManager::Get().IsValid(GameplayTag)) \
{ \
UE_LOG(LogTemp, Error, TEXT("LevelTravel -plugin: Invalid GameplayTag: %s"), *GameplayTag.ToString()); \
return false; \
}

UCLASS()
class ULevelTravelFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
