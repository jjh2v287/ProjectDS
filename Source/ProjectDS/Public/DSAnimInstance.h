// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDS_API UDSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = DSAnimInstance)
	float Speed = 0.f;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
