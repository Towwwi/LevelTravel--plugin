// Copyright Tommi Kekomäki 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/GameModeBase.h"
#include "LevelTravelManagerComponent.generated.h"

struct FGameplayTag;
class ULevelData;
class AGameModeBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSeamlessTravelEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestClientTravelRules, const FGameplayTag&, LevelGameplayTag);


/**
 *  Main Component to control modular-level traveling
 *  !! Should only exist only on gamemode so has always authority
 */
UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class ULevelTravelManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	ULevelTravelManagerComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	//! @brief Apply rules on blueprint to see if the client can request a Server Travel. If it returns false, we are not yet ready to travel
	bool RequestClientTravelRules(const FGameplayTag& LevelGameplayTag);

	//! @brief Find level's path by GameplayTag
	UFUNCTION(BlueprintCallable, Category="LevelTravelManager")
	FString GetLevelPath(const FGameplayTag& LevelGameplayTag) const;
	
	UFUNCTION(BlueprintCallable, Category="LevelTravelManager")
	void ServerStartTravelToLevel(const FGameplayTag& LevelGameplayTag);

	//! @brief Get the timer until ServerTravel
	float GetTimerFromLevelData(const FGameplayTag& GameplayTag) const;

	//! @brief Called from LevelTravelClientComponent to add its owning controller to list for traveling
	void AddPlayerControllerForServerTravelList(const APlayerController* PlayerController, const FGuid& NetGuid, const FGameplayTag& LevelGameplayTag);
	
protected:
	UFUNCTION()
	void ServerTravelToLevel(const FLevelDataStruct& LevelData, const FGameplayTag& LevelGameplayTag) const;
	
private:
	UFUNCTION()
	virtual void OnSeamlessTravelStartEvent(UWorld* CurrentWorld, const FString& LevelName);

	UFUNCTION()
	virtual void OnSeamlessTravelTransition(UWorld* CurrentWorld);
	
	UFUNCTION()
	void StartTimer(float TimerDuration);
	
	void OnTimerElapsed();

	void StartClientRequestTravel(const FGameplayTag& LevelGameplayTag);
#pragma region Events
protected:
	FDelegateHandle OnSeamlessTravelStart;
	FDelegateHandle OnSeamlessTravelEnd;

	UPROPERTY()
	FOnSeamlessTravelEvent OnSeamlessTravelEvent;
	
	UPROPERTY(BlueprintAssignable, Category="LevelTravelManager")
	FOnRequestClientTravelRules OnRequestClientTravelRules;
#pragma endregion Events

public:
	UPROPERTY(EditAnywhere, Category="LevelTravelManager")
	ULevelData* LevelDataAsset;
	
	UPROPERTY(BlueprintReadWrite, Category="LevelTravelManager")
	bool ClientTravelRulesAccepted = true;

	//! @brief When client requests to travel we add the controllers here
	UPROPERTY(BlueprintReadOnly, Category="LevelTravelManager")
	TMap<FGuid, APlayerController*> PlayerControllersToTravel;
	
	//! @brief How many clients have requested a server travel
	UPROPERTY(BlueprintReadOnly, Category="LevelTravelManager")
	int32 ClientTravelRequestAmount = 0;

	//! @brief This is the tag of the current traveled level
	UPROPERTY(BlueprintReadOnly, Category="LevelTravelManager")
	FGameplayTag CurrentLevelGameplayTag;

private:
	FTimerHandle TimerHandle;
	
	bool bTimerFinished = false;
		
	TObjectPtr<AGameModeBase> GamemodeBase;

	float FadeTime = 5;

};
