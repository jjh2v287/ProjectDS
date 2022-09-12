// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeAttackNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDS_API UMeleeAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageTrace)
	TWeakObjectPtr<UAnimMontage> Montage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageTrace)
	FName StartSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageTrace)
	FName EndSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageTrace)
	float Radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageTrace)
	float FPS;

	float fStartTime;
	float fEndTime;
	float fDurationTime;
	float fPerTime;
	float fCurrentTime;

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration);
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime);
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);

	virtual void BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload);
	virtual void BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime);
	virtual void BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload);

	void HitCheck(FBranchingPointNotifyPayload& BranchingPointPayload, FLinearColor Color);
	FTransform ConvertLocalTransformToWorld(const FTransform& LocalTransform, const FTransform& ParentTransform);
};
