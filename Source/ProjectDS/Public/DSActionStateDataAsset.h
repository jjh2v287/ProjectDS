// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DSActionStateDataAsset.generated.h"

class UDSActionState;

/**
 * 
 */
UCLASS()
class PROJECTDS_API UDSActionStateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = ActionStates)
	FGameplayTag IdleTag;

	UPROPERTY(EditAnywhere, Category = ActionStates)
	TSubclassOf<class UDSActionState> IdleAction;

	UPROPERTY(EditAnywhere, Category = ActionStates, meta = (DisplayThumbnail = "true"))
	TMap<FGameplayTag, TSubclassOf<class UDSActionState>> ActionState;
	//TObjectPtr<class UDSActionState> ActionState;
};
