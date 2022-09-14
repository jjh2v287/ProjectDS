// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DSCharacter.generated.h"

class UDSActionState;
class UDSActionStateDataAsset;
class UDSActionStateComponent;

UCLASS()
class PROJECTDS_API ADSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADSCharacter();

public:
	UPROPERTY(EditAnywhere, Category = ActionStates)
	TObjectPtr<UDSActionStateDataAsset> ActionStateDataAsset;

protected:
	TObjectPtr<UDSActionStateComponent> ActionStateComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = ActionState)
	UDSActionStateComponent* GetActionStateComponent();
};
