// Copyright Tommi Kekomäki 2023. All Rights Reserved.

#include "LevelTravelClient.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "LevelTravelFunctionLibrary.h"
#include "LevelTravelManagerComponent.h"
#include "Net/UnrealNetwork.h"

ULevelTravelClient::ULevelTravelClient(): PlayerController(nullptr), LevelTravelManager(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULevelTravelClient::SetFadeDuration_Implementation(float FadeDuration)
{
	NewFadeTime = FadeDuration;
}

void ULevelTravelClient::CameraFade_Implementation()
{
	if (const APlayerController* NewPlayerController = Cast<APlayerController>(GetOwner()))
	{
		NewPlayerController->PlayerCameraManager->StartCameraFade(0,1,NewFadeTime, FLinearColor::Black, true, false);
	}
}

void ULevelTravelClient::BeginPlay()
{
	Super::BeginPlay();
	
	if (const APlayerController* NewPlayerController = Cast<APlayerController>(GetOwner()))
	{
		PlayerController = NewPlayerController;

		if(!IsValid(PlayerController.Get()))
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid, called from LevelTravelClient") );
			return;
		}
	}

	if(GetOwnerRole() != ROLE_Authority || NetGuid.IsValid())
		return;

	NetGuid = FGuid::NewGuid();
}

void ULevelTravelClient::InitializeComponent()
{
	Super::InitializeComponent();
}

void ULevelTravelClient::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NewFadeTime);
}

void ULevelTravelClient::RequestServerToTravel_Implementation(const FGameplayTag& LevelGameplayTag)
{
	if(!LevelGameplayTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("LevelTravelClient, Controller or LevelGameplayTag is not valid on RequestServerToTravel") );
		return;
	}
	
	LevelTravelManager = ULevelTravelFunctionLibrary::GetLevelTravelManagerComponent(GetWorld());

	//If this player has already requested the travel
	if(LevelTravelManager->PlayerControllersToTravel.Contains(NetGuid))
		return;
	
	LevelTravelManager->AddPlayerControllerForServerTravelList(PlayerController.Get(), NetGuid, LevelGameplayTag);
}

