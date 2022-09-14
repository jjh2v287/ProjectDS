// Fill out your copyright notice in the Description page of Project Settings.


#include "DSCharacter.h"
#include "DSActionStateComponent.h"
#include "DSActionStateDataAsset.h"
#include "DSActionState.h"

// Sets default values
ADSCharacter::ADSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ActionStateComponent = CreateDefaultSubobject<UDSActionStateComponent>(TEXT("SActionStateComponent"));
}

// Called when the game starts or when spawned
void ADSCharacter::BeginPlay()
{
	Super::BeginPlay();
	ActionStateComponent->InitPreset(ActionStateDataAsset, this);
}

// Called every frame
void ADSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UDSActionStateComponent* ADSCharacter::GetActionStateComponent()
{
	return ActionStateComponent;
}

