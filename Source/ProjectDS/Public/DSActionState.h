// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DSActionState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTDS_API UDSActionState : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = ActionStates)
	FGameplayTag Tag;
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = ActionState)
	void OnBegine();

	UFUNCTION(BlueprintNativeEvent, Category = ActionState)
	void OnTick();

	UFUNCTION(BlueprintNativeEvent, Category = ActionState)
	void OnEnd();

	UFUNCTION(BlueprintNativeEvent, Category = ActionState)
	void OnAbort();
};
