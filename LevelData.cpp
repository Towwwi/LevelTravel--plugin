// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelData.h"

#if WITH_EDITOR
EDataValidationResult ULevelData::IsDataValid(FDataValidationContext& Context)
{
	EDataValidationResult Result = EDataValidationResult::NotValidated;
	
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE