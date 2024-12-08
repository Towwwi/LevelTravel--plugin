// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelTravelClient.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "LevelTravelManagerComponent.h"
#include "Net/UnrealNetwork.h"

ULevelTravelClient::ULevelTravelClient(): PlayerController(nullptr), LevelTravelManager(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
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

	if(GetOwnerRole() != ROLE_Authority)
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
}

void ULevelTravelClient::CameraFade_Implementation()
{
	PlayerController->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 5., FColor:: Black, true, true);
}

void ULevelTravelClient::RequestServerToTravel_Implementation(const FGameplayTag& LevelGameplayTag)
{
	if(!LevelGameplayTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("LevelTravelClient, Controller or LevelGameplayTag is not valid on RequestServerToTravel") );
		return;
	}
	
	LevelTravelManager = GetWorld()->GetAuthGameMode()->FindComponentByClass<ULevelTravelManagerComponent>();
	if(!IsValid(LevelTravelManager.Get()))
	{
		UE_LOG(LogTemp, Error, TEXT("LevelTravelManagerComponent is not valid, called from LevelTravelClient") );
		return;
	}

	//If this player has already requested the travel
	if(LevelTravelManager->PlayerControllersToTravel.Contains(NetGuid))
		return;
	
	LevelTravelManager->AddPlayerControllerForServerTravelList(PlayerController.Get(), NetGuid, LevelGameplayTag);
}
