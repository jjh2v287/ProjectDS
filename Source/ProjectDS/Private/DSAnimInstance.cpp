// Fill out your copyright notice in the Description page of Project Settings.


#include "DSAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UDSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	TWeakObjectPtr<APawn> Pawn = TryGetPawnOwner();
	if (Pawn.Get())
	{
		Speed = Pawn->GetVelocity().Size();
	}
}
