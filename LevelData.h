// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "LevelData.generated.h"



USTRUCT()
struct FLevelDataStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Level info")
	TSoftObjectPtr<UWorld> Level;
	
	//Should we use seamless travel if possible to travel this level
	UPROPERTY(EditAnywhere, Category="Traveling")
	bool bUseSeamlessTravel = true;

	// Do we want to notify the game after traveling has finished?
	UPROPERTY(EditAnywhere, Category="Traveling")
	bool bShouldSkipGameNotify = false;
	
	// Only used on ClientTravel
	// Travel_Absolute -> Corresponds to a non-seamless travel (Hard reset)
	// Travel_Partial  -> Corresponds to a non-seamless travel (Soft reset)
	// Travel_Relative -> Corresponds to a seamless travel
	UPROPERTY(EditAnywhere, Category="ClientTravel TravelType")
	TEnumAsByte<ETravelType> TravelType = TRAVEL_Relative;

	// Should we travel after a certain time?
	UPROPERTY(EditAnywhere, Category="Traveling")
	bool bUseTimerToTravel = false;

	UPROPERTY(EditAnywhere, Category="Traveling", meta = (EditCondition = "bUseTimerToTravel"))
	bool bUseFade = false;
	
	// If UseTimerToTravel is true, this is the time amount to wait until travel happens
	UPROPERTY(EditAnywhere, Category="Traveling", meta = (EditCondition = "bUseTimerToTravel"))
	float Timer;
};

/**
 * 
 */
UCLASS()
class ULevelData : public UDataAsset
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) override;
#endif

public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FLevelDataStruct> LevelDatas;
};

