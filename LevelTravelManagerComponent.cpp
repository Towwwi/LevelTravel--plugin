// Copyright Tommi Kekomäki 2023. All Rights Reserved.

#include "LevelTravelManagerComponent.h"
#include "GameplayTagContainer.h"
#include "LevelData.h"
#include "LevelTravelClient.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"

ULevelTravelManagerComponent::ULevelTravelManagerComponent(): LevelDataAsset(nullptr), ClientTravelRulesAccepted(false),
                                                              CurrentLevelGameplayTag(),
                                                              GamemodeBase(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULevelTravelManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	#if WITH_EDITOR
	GEngine->Exec( GetWorld(), TEXT( "net.AllowPIESeamlessTravel 1" ) ); // @TODO Tommi, Do on initialization
	#endif
	
	if(const TObjectPtr<AGameModeBase> GameMode = Cast<AGameModeBase>(GetOwner()))
	{
		GamemodeBase = GameMode;
		
		// FGameModeEvents::OnGameModePostLoginEvent().AddUObject(this, &ULevelTravelManagerComponent::OnPostLoginEvent); Used for hard travel (Might be usefull)
	}
	
	OnSeamlessTravelStart = FWorldDelegates::OnSeamlessTravelStart.AddUObject(this, &ThisClass::OnSeamlessTravelStartEvent);
	OnSeamlessTravelEnd = FWorldDelegates::OnSeamlessTravelTransition.AddUObject(this, &ThisClass::OnSeamlessTravelTransition);
	
	PlayerControllersToTravel.Reset();
	ClientTravelRequestAmount = 0;
	
}

bool ULevelTravelManagerComponent::RequestClientTravelRules(const FGameplayTag& LevelGameplayTag)
{
	ClientTravelRequestAmount ++;
	
	if (!OnRequestClientTravelRules.IsBound())
		return ClientTravelRulesAccepted;

	OnRequestClientTravelRules.Broadcast(LevelGameplayTag);
	return ClientTravelRulesAccepted;
}


FString ULevelTravelManagerComponent::GetLevelPath(const FGameplayTag& LevelGameplayTag) const
{

	// @TODO Tommi, Create macro for validating data
	if(const FLevelDataStruct* LevelData = LevelDataAsset->LevelDatas.Find(LevelGameplayTag))
	{
			FString PackagePath = LevelData->Level.GetLongPackageName();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *PackagePath);

			return PackagePath;
	}
	
	FString PackagePath;
	return PackagePath;
}

void ULevelTravelManagerComponent::OnSeamlessTravelStartEvent(UWorld* CurrentWorld, const FString& LevelName)
{
  //@TODO Tommi - Not sure if we need any event here but could be useful for maybe starting a ASYNC loading screen? 
}

void ULevelTravelManagerComponent::OnSeamlessTravelTransition(UWorld* CurrentWorld)
{
	//@TODO Tommi - Not sure if we need any event here but could be useful
	// This is too early though to do any actual gameplay related logic
}

void ULevelTravelManagerComponent::ServerStartTravelToLevel(const FGameplayTag& LevelGameplayTag)
{
	if(!LevelDataAsset)
	{
		if(GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("LevelTravelManager - LevelData is null"));	
	}
	
	if(const FLevelDataStruct* LevelData = LevelDataAsset->LevelDatas.Find(LevelGameplayTag))
	{
		if(!IsValid(GetWorld()))
			return;

		CurrentLevelGameplayTag = LevelGameplayTag;
		
		if(LevelData->bUseTimerToTravel && !bTimerFinished)
		{
			StartTimer(LevelData->Timer);
			return;
		}
			ServerTravelToLevel(*LevelData, LevelGameplayTag);
	}
}

float ULevelTravelManagerComponent::GetTimerFromLevelData(const FGameplayTag& LevelGameplayTag) const
{
	const FLevelDataStruct* LevelData = LevelDataAsset->LevelDatas.Find(LevelGameplayTag);
	if(LevelData == nullptr)
		return 0.0f;
	
	if (LevelData->bUseTimerToTravel)
		return LevelData->Timer;
	
	return 0.0f;
}

void ULevelTravelManagerComponent::AddPlayerControllerForServerTravelList(const APlayerController* PlayerController, const FGuid& NetGuid, const FGameplayTag& LevelGameplayTag)
{
	if(IsValid(PlayerController))
	{
		PlayerControllersToTravel.Add(NetGuid, const_cast<APlayerController*>(PlayerController));
		StartClientRequestTravel(LevelGameplayTag);
	}
}

void ULevelTravelManagerComponent::ServerTravelToLevel(const FLevelDataStruct& LevelData,
                                                       const FGameplayTag& LevelGameplayTag) const
{
	GetWorld()->ServerTravel(GetLevelPath(LevelGameplayTag), LevelData.bUseSeamlessTravel, LevelData.bShouldSkipGameNotify);
}

void ULevelTravelManagerComponent::StartTimer(float TimerDuration) //@TODO Tommi, think if net-sync clock is needed.
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnTimerElapsed, TimerDuration, false);
}

void ULevelTravelManagerComponent::OnTimerElapsed()
{
	bTimerFinished = true;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	ServerStartTravelToLevel(CurrentLevelGameplayTag);
}

void ULevelTravelManagerComponent::StartClientRequestTravel(const FGameplayTag& LevelGameplayTag)
{
	if(!RequestClientTravelRules(LevelGameplayTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelTravelManagerComponent rules deny traveling or all the requests are not yet complited") );
		return;
	}
	
	const FLevelDataStruct* LevelData = LevelDataAsset->LevelDatas.Find(LevelGameplayTag);
	if(LevelData == nullptr)
		return;
	
	ServerStartTravelToLevel(LevelGameplayTag);

	if(LevelData->bUseFade)
	{
		for (const TPair<FGuid, APlayerController*> Pair : PlayerControllersToTravel)
		{
			const APlayerController* InPlayerController = Pair.Value;
			if (IsValid(InPlayerController))
			{
				InPlayerController->GetComponentByClass<ULevelTravelClient>()->SetFadeDuration(LevelData->Timer);
				InPlayerController->GetComponentByClass<ULevelTravelClient>()->CameraFade();
			}
		}
	}
}

