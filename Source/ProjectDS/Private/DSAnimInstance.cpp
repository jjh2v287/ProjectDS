// Fill out your copyright notice in the Description page of Project Settings.


#include "DSAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UDSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (TryGetPawnOwner())
	{
		if (TryGetPawnOwner()->GetMovementComponent())
		{
			Speed = TryGetPawnOwner()->GetMovementComponent()->Velocity.Size();
		}
	}
}
