// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DSActionStateComponent.generated.h"

class UDSActionState;
class UDSActionStateDataAsset;
class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTDS_API UDSActionStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDSActionStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UDSActionState>> ActionStates;

	UPROPERTY()
	FGameplayTag IdleTag;

	UPROPERTY()
	UDSActionState* ActiveActionStates;

	UPROPERTY()
	ACharacter* OwnerCharacter;

public:
	void InitPreset(TWeakObjectPtr<UDSActionStateDataAsset> ActionStateDataAsset, ACharacter* Character);
		
};
