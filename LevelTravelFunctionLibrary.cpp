// Copyright Tommi Kekomäki 2023. All Rights Reserved.


#include "LevelTravelFunctionLibrary.h"

#include "LevelTravelManagerComponent.h"
#include "GameFramework/GameStateBase.h"

int32 ULevelTravelFunctionLibrary::GetCurrentPlayerCount(UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
		return false;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
		return false;

	const AGameStateBase* GameState = World->GetGameState();
	if (!IsValid(GameState))
		return 0;

	return GameState->PlayerArray.Num();
}

bool ULevelTravelFunctionLibrary::IsTravelRequestAmountEqualToPlayerCount(UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
		return false;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
		return false;
	
	return GetLevelTravelManagerComponent(WorldContextObject)->ClientTravelRequestAmount >= GetCurrentPlayerCount(WorldContextObject);
}

ULevelTravelManagerComponent* ULevelTravelFunctionLibrary::GetLevelTravelManagerComponent(UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
		return nullptr;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
		return nullptr;
	
	if (!IsValid(World->GetAuthGameMode()->FindComponentByClass<ULevelTravelManagerComponent>()))
	{
		UE_LOG(LogTemp, Error, TEXT("LevelTravelManagerComponent is not valid, called from LevelTravelClient") );
		return nullptr;
	}
	
	return World->GetAuthGameMode()->FindComponentByClass<ULevelTravelManagerComponent>();

}

FLevelDataStruct ULevelTravelFunctionLibrary::GetCurrentLevelData(UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
		return FLevelDataStruct{};

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
		return FLevelDataStruct{};
	
	if(const FLevelDataStruct* LevelData = GetLevelTravelManagerComponent(WorldContextObject)->LevelDataAsset->LevelDatas.Find(GetLevelTravelManagerComponent(WorldContextObject)->CurrentLevelGameplayTag))
	{
		return *LevelData;
	}

	return FLevelDataStruct{};
}

FGameplayTag ULevelTravelFunctionLibrary::GetCurrentLevelGameplayTag(UObject* WorldContextObject)
{
	if(!IsValid(WorldContextObject))
		return FGameplayTag::EmptyTag;

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if(!IsValid(World))
		return FGameplayTag::EmptyTag;

	return GetLevelTravelManagerComponent(WorldContextObject)->CurrentLevelGameplayTag;
}
