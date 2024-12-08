// Copyright Tommi Kekomäki 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelTravelClient.generated.h"

class ULevelTravelManagerComponent;

/**
 *  Component for a player-controller to request traveling to levels
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULevelTravelClient : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelTravelClient();


	UFUNCTION(Server, Reliable)
	void SetFadeDuration(float FadeDuration);
	
	UFUNCTION(Client,Reliable)
	void CameraFade();
	
protected:
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	//Ask the server to travel from the PlayerController.
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestServerToTravel(const FGameplayTag& LevelGameplayTag);

private:
	TSoftObjectPtr<APlayerController> PlayerController;

	TSoftObjectPtr<ULevelTravelManagerComponent> LevelTravelManager;
	
	FGuid NetGuid;

	UPROPERTY(Replicated)
	float NewFadeTime = 5;
};

