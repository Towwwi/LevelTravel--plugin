// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelTravelClient.generated.h"

class ULevelTravelManagerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULevelTravelClient : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelTravelClient();

	UFUNCTION(Client,Reliable)
	void CameraFade();
	
protected:
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	//Ask server to do a server travel from the PlayerController.
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestServerToTravel(const FGameplayTag& LevelGameplayTag);

private:
	TSoftObjectPtr<APlayerController> PlayerController;

	TSoftObjectPtr<ULevelTravelManagerComponent> LevelTravelManager;
	
	FGuid NetGuid;
	
};
