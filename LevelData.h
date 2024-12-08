// Copyright Tommi Kekomäki 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "LevelData.generated.h"

USTRUCT(Blueprintable)
struct FLevelDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Level info", BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
	
	//Should we use seamless travel if possible to travel this level
	UPROPERTY(EditAnywhere, Category="Traveling", BlueprintReadOnly)
	bool bUseSeamlessTravel = true;

	// Do we want to notify the game after traveling has finished?
	UPROPERTY(EditAnywhere, Category="Traveling", BlueprintReadOnly)
	bool bShouldSkipGameNotify = false;
	
	// Only used on ClientTravel
	// Travel_Absolute -> Corresponds to a non-seamless travel (Hard reset)
	// Travel_Partial  -> Corresponds to a non-seamless travel (Soft reset)
	// Travel_Relative -> Corresponds to a seamless travel
	UPROPERTY(EditAnywhere, Category="ClientTravel TravelType", BlueprintReadOnly)
	TEnumAsByte<ETravelType> TravelType = TRAVEL_Relative;

	// Should we travel after a certain time?
	UPROPERTY(EditAnywhere, Category="Traveling", BlueprintReadOnly)
	bool bUseTimerToTravel = false;

	UPROPERTY(EditAnywhere, Category="Traveling", meta = (EditCondition = "bUseTimerToTravel"), BlueprintReadOnly)
	bool bUseFade = false;
	
	// If UseTimerToTravel is true, this is the time amount to wait until travel happens
	UPROPERTY(EditAnywhere, Category="Traveling", meta = (EditCondition = "bUseTimerToTravel"), BlueprintReadOnly)
	float Timer;
};

/**
 *  Data-asset to control how should we travel to the wanted level
 */
UCLASS()
class ULevelData : public UDataAsset
{
	GENERATED_BODY()

	//@TODO Tommi - Proper data-validation
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) override;
#endif

public:
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FLevelDataStruct> LevelDatas;
};
