// Fill out your copyright notice in the Description page of Project Settings.


#include "DSNPCCharacter.h"
#include "DSCharacterMovementComponent.h"

// Sets default values
ADSNPCCharacter::ADSNPCCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ADSNPCCharacter::ADSNPCCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDSCharacterMovementComponent>(CharacterMovementComponentName))
{
}

// Called when the game starts or when spawned
void ADSNPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADSNPCCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADSNPCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

