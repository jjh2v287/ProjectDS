// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttackNotifyState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "AnimationUtils.h"

static TAutoConsoleVariable<int32> ShowDamageHitTrace(
	TEXT("ShowDamageHitTrace"),
	0,
	TEXT("0: Disable, 1: Enable"),
	ECVF_Default
);

void UMeleeAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{

}

void UMeleeAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{

}

void UMeleeAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

}

void UMeleeAttackNotifyState::BranchingPointNotifyBegin(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	USkeletalMeshComponent* MyMesh = BranchingPointPayload.SkelMeshComponent;
	if (!MyMesh)
		return;

	AActor* Owner = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (!Owner)
		return;

	UAnimInstance* animInstance = MyMesh->GetAnimInstance();
	if (!animInstance)
		return;

	//FAnimMontageInstance* currMontage = animInstance->GetActiveMontageInstance();
	//if (!currMontage)
	//	return;

	Montage = animInstance->GetActiveMontageInstance()->Montage;
	if (!Montage.IsValid())
		return;

	fStartTime = BranchingPointPayload.NotifyEvent->GetTime();
	fDurationTime = BranchingPointPayload.NotifyEvent->GetDuration();
	fEndTime = BranchingPointPayload.NotifyEvent->EndLink.GetTime();// LinkValue;
	fCurrentTime = fStartTime;
	fPerTime = fCurrentTime;
}

void UMeleeAttackNotifyState::BranchingPointNotifyTick(FBranchingPointNotifyPayload& BranchingPointPayload, float FrameDeltaTime)
{
	fPerTime = fCurrentTime;
	fCurrentTime += FrameDeltaTime;
	HitCheck(BranchingPointPayload, FLinearColor::Red);
}

void UMeleeAttackNotifyState::BranchingPointNotifyEnd(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (fCurrentTime < fEndTime)
	{
		fPerTime = fCurrentTime;
		fCurrentTime = fEndTime;
		HitCheck(BranchingPointPayload, FLinearColor::Green);
	}
}

void UMeleeAttackNotifyState::HitCheck(FBranchingPointNotifyPayload& BranchingPointPayload, FLinearColor Color)
{
	USkeletalMeshComponent* MyMesh = BranchingPointPayload.SkelMeshComponent;
	if (!MyMesh)
		return;

	AActor* Owner = BranchingPointPayload.SkelMeshComponent->GetOwner();
	if (!Owner)
		return;

	UAnimInstance* animInstance = MyMesh->GetAnimInstance();
	if (!animInstance)
		return;

	//FAnimMontageInstance* currMontage = animInstance->GetActiveMontageInstance();
	//if (!currMontage)
	//	return;

	if (!Montage.IsValid())
		return;

	UAnimSequence* AnimSeq = Cast<UAnimSequence>(Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].AnimReference);
	if (!AnimSeq)
		return;

	USkeleton* Skeleton = Montage->GetSkeleton();
	if (!Skeleton)
		return;

	FName boneName = MyMesh->GetSocketBoneName(StartSocketName);
	int StartBoneIndex = Skeleton->GetReferenceSkeleton().FindBoneIndex(boneName);
	boneName = MyMesh->GetSocketBoneName(EndSocketName);
	int EndBoneIndex = Skeleton->GetReferenceSkeleton().FindBoneIndex(boneName);
	if (StartBoneIndex < 0 || EndBoneIndex < 0)
		return;

	FTransform StartSocketTransform = MyMesh->GetSocketTransform(StartSocketName, RTS_ParentBoneSpace);
	FTransform EndSocketTransform = MyMesh->GetSocketTransform(EndSocketName, RTS_ParentBoneSpace);
	FTransform RelativeTransform = MyMesh->GetComponentTransform().GetRelativeTransform(Owner->GetTransform());

	// 본 정보 가져오기
	TArray<FBoneData> OutBoneData;
	FAnimationUtils::BuildSkeletonMetaData(Skeleton, OutBoneData);

	float delta = fCurrentTime - fPerTime;
	float addTime = 1.0f / FPS;
	int count = FMath::CeilToFloat(delta / addTime);
	for (int i = 0; i < count; i++)
	{
		float AnimTime = fPerTime + (i * addTime);

		FTransform StartTransform, EndTransform;

		{// Start 소켓
			// 메쉬 컴포넌트 로컬 행렬 곱
			StartTransform = RelativeTransform * StartTransform;
			StartTransform.NormalizeRotation();

			for (int j = OutBoneData[StartBoneIndex].BonesToRoot.Num() - 2; j >= 0; j--)
			{
				// 본 계층 구조 모두 행렬 곱
				int Track = OutBoneData[StartBoneIndex].BonesToRoot[j];
				FTransform BoneTransform = FTransform::Identity;
				AnimSeq->ExtractBoneTransform(AnimSeq->GetRawAnimationTrack(Track), BoneTransform, AnimTime);

				// UE5
				//float SequenceLength = AnimSeq->GetPlayLength();
				//const FBoneAnimationTrack TrackData = AnimSeq->GetDataModel()->GetBoneTrackByIndex(Track);
				//const FRawAnimSequenceTrack& RawTrack = TrackData.InternalTrackData;
				//const int32 TotalFrames = FMath::Max(RawTrack.PosKeys.Num(), RawTrack.RotKeys.Num());
				//FTransform BoneTransform2 = FTransform::Identity;
				//FAnimationUtils::ExtractTransformFromTrack(AnimTime, TotalFrames, SequenceLength, RawTrack, EAnimInterpolationType::Linear, BoneTransform2);

				StartTransform = BoneTransform * StartTransform;
				StartTransform.NormalizeRotation();
			}

			// Find한 본 계산
			FTransform FindBoneTransform;
			AnimSeq->ExtractBoneTransform(AnimSeq->GetRawAnimationTrack(StartBoneIndex), FindBoneTransform, AnimTime);
			StartTransform = FindBoneTransform * StartTransform;
			StartTransform.NormalizeRotation();

			// Find한 소캣 로컬 행렬 곱
			StartTransform = StartSocketTransform * StartTransform;
			StartTransform.NormalizeRotation();
		}

		{// End 소켓
			// 메쉬 컴포넌트 로컬 행렬 곱
			EndTransform = RelativeTransform * EndTransform;
			EndTransform.NormalizeRotation();

			for (int j = OutBoneData[EndBoneIndex].BonesToRoot.Num() - 2; j >= 0; j--)
			{
				// 본 계층 구조 모두 행렬 곱
				int Track = OutBoneData[EndBoneIndex].BonesToRoot[j];
				FTransform BoneTransform;
				AnimSeq->ExtractBoneTransform(AnimSeq->GetRawAnimationTrack(Track), BoneTransform, AnimTime);

				EndTransform = BoneTransform * EndTransform;
				EndTransform.NormalizeRotation();
			}

			// Find한 본 계산
			FTransform FindBoneTransform;
			AnimSeq->ExtractBoneTransform(AnimSeq->GetRawAnimationTrack(EndBoneIndex), FindBoneTransform, AnimTime);
			EndTransform = FindBoneTransform * EndTransform;
			EndTransform.NormalizeRotation();

			// Find한 소캣 로컬 행렬 곱
			EndTransform = EndSocketTransform * EndTransform;
			EndTransform.NormalizeRotation();
		}

		// 액터 월드 행영 적용
		FVector StartFinalBoneTransform = Owner->GetActorRotation().RotateVector(StartTransform.GetLocation());
		StartFinalBoneTransform = StartFinalBoneTransform + Owner->GetTransform().GetLocation();

		FVector EndFinalBoneTransform = Owner->GetActorRotation().RotateVector(EndTransform.GetLocation());
		EndFinalBoneTransform = EndFinalBoneTransform + Owner->GetTransform().GetLocation();

		TArray<AActor*> DamagedActors;
		TArray<FHitResult> HitResults;
		TEnumAsByte<ETraceTypeQuery> TraceChannel = { TraceTypeQuery1 };
		EDrawDebugTrace::Type DrawDebugTraceType = ShowDamageHitTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		if (UKismetSystemLibrary::SphereTraceMulti(Owner, StartFinalBoneTransform, EndFinalBoneTransform, Radius, TraceChannel, false, DamagedActors, DrawDebugTraceType, HitResults, true, Color, FLinearColor::Blue, 10.0f))
		{
			for (const FHitResult& HitResult : HitResults)
			{
			}
		}
	}

	{
		// 테스트용 소켓 현제 위치
		FVector StartLocation = MyMesh->GetSocketLocation(EndSocketName);
		TArray<AActor*> DamagedActors;
		TArray<FHitResult> HitResults;
		TEnumAsByte<ETraceTypeQuery> TraceChannel = { TraceTypeQuery1 };
		EDrawDebugTrace::Type DrawDebugTraceType = ShowDamageHitTrace.GetValueOnGameThread() == 1 ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
		FVector LastLocation = MyMesh->GetSocketLocation(StartSocketName);
		if (UKismetSystemLibrary::SphereTraceMulti(Owner, LastLocation, StartLocation, Radius, TraceChannel, false, DamagedActors, DrawDebugTraceType, HitResults, true, FLinearColor::Black, FLinearColor::White, 10.0f))
		{
			for (const FHitResult& HitResult : HitResults)
			{
			}
		}
	}
}

FTransform UMeleeAttackNotifyState::ConvertLocalTransformToWorld(const FTransform& LocalTransform, const FTransform& ParentTransform)
{
	FTransform OutTransform;

	FVector OutLoc = UKismetMathLibrary::GreaterGreater_VectorRotator(LocalTransform.GetLocation(), (ParentTransform.GetRotation().Rotator())) + ParentTransform.GetLocation();
	FRotator OutRot = FRotator(FQuat(LocalTransform.GetRotation()) * FQuat(ParentTransform.GetRotation()));
	//FVector OutScale = LocalTransform.GetScale3D() * ParentTransform.GetScale3D();
	FVector OutScale = ParentTransform.GetScale3D() * LocalTransform.GetScale3D();

	OutTransform.SetLocation(OutLoc);
	OutTransform.SetRotation(OutRot.Quaternion());
	OutTransform.SetScale3D(OutScale);

	return OutTransform;
}