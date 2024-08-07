// Tommi Kekomaki

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

UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class ULevelTravelManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	ULevelTravelManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="LevelTravelManager")
	int32 GetCurrentPlayerCount();
	
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
	void ServerTravelToLevel(const FLevelDataStruct LevelData, const FGameplayTag& LevelGameplayTag) const;
	
private:
	UFUNCTION()
	virtual void OnSeamlessTravelStartEvent(UWorld* CurrentWorld, const FString& LevelName);

	UFUNCTION()
	virtual void OnSeamlessTravelTransition(UWorld* CurrentWorld);
	
	UFUNCTION()
	void StartTimer(float TimerDuration);
	
	void OnTimerElapsed();

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

protected:
	//! @brief How many clients have requested a server travel
	UPROPERTY(BlueprintReadOnly, Category="LevelTravelManager")
	int32 ClientTravelRequestAmount = 0;

private:
	FGameplayTag CurrentLevelGameplayTag;
	
	FTimerHandle TimerHandle;
	
	bool bTimerFinished = false;
		
	TObjectPtr<AGameModeBase> GamemodeBase;
};
