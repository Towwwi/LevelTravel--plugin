
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "LevelData.h"
#include "LevelTravelFunctionLibrary.generated.h"

#define Cast_TO_WORLD(World) \
if (!IsValid(World)) \
{ \
return nullptr; \
} \
UWorld* WorldObject = Cast<UWorld>(World); \
if (WorldObject == nullptr) \
{ \
return nullptr; \
} \
WorldObject

struct FLevelDataStruct;
class ULevelTravelManagerComponent;

UCLASS()
class ULevelTravelFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure,  meta = (WorldContext = "WorldContextObject"))
	static int32 GetCurrentPlayerCount(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="ModularLevelTravelFunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static bool IsTravelRequestAmountEqualToPlayerCount(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ModularLevelTravelFunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static ULevelTravelManagerComponent* GetLevelTravelManagerComponent(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ModularLevelTravelFunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static FLevelDataStruct GetCurrentLevelData(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ModularLevelTravelFunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static FGameplayTag GetCurrentLevelGameplayTag(UObject* WorldContextObject);
};
