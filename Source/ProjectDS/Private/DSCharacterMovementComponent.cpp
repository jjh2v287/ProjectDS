// Fill out your copyright notice in the Description page of Project Settings.


#include "DSCharacterMovementComponent.h"

UDSCharacterMovementComponent::UDSCharacterMovementComponent()
{
	PrimaryComponentTick.bRunOnAnyThread = true;
	DefaultLandMovementMode = EMovementMode::MOVE_Custom;
	DefaultWaterMovementMode = EMovementMode::MOVE_Custom;
}
