// Fill out your copyright notice in the Description page of Project Settings.

#include "SFTickLodComponent.h"
#include "Async/ParallelFor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
USFTickLodComponent::USFTickLodComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USFTickLodComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	if (UWorld* World = GetWorld())
	{
		if (UTickLodSubsystem* Subsystem = World->GetSubsystem<UTickLodSubsystem>())
		{
			Subsystem->RegisterAgent(this);
		}
	}

	if (AActor* Owner = GetOwner())
	{
		for (UActorComponent* Component : Owner->GetComponents())
		{
			if (USkinnedMeshComponent* SkinnedMeshComponent = Cast<USkinnedMeshComponent>(Component))
			{
				SkinnedMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
				SkinnedMeshComponent->bEnableUpdateRateOptimizations = true;
				SkinnedMeshComponent->AnimUpdateRateParams->bShouldUseLodMap = false;
				SkinnedMeshComponent->AnimUpdateRateParams->BaseNonRenderedUpdateRate = FramesSkippedScreenSizeThresholds.Num() + 1;
				SkinnedMeshComponent->AnimUpdateRateParams->MaxEvalRateForInterpolation = FramesSkippedScreenSizeThresholds.Num() + 1;

				SkinnedMeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Empty(FramesSkippedScreenSizeThresholds.Num());
				for (const float ScreenSize : FramesSkippedScreenSizeThresholds)
				{
					SkinnedMeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Add(FMath::Square(ScreenSize));
				}
			}

			if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(Component))
			{
				MovementComponent->bEnablePhysicsInteraction = false;
			}
		}
	}
}


// Called every frame
void USFTickLodComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region UTickLodSubsystem

ETickableTickType UTickLodSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}
	return ETickableTickType::Always;
}

TStatId UTickLodSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTickLodSubsystem, STATGROUP_Tickables);
}

void UTickLodSubsystem::Tick(float DeltaTime)
{
	//APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		UCameraComponent* Camera = Cast<UCameraComponent>(Player->GetComponentByClass(UCameraComponent::StaticClass()));
		if (Camera)
		{
			FVector CameraLocation = Camera->GetComponentToWorld().GetLocation();
			FVector CameraForward = Camera->GetComponentToWorld().GetRotation().Vector();

			FVector LineEnd = CameraLocation + (CameraForward * 300.f);
			LineEnd.Z = CameraLocation.Z;
			//UKismetSystemLibrary::DrawDebugArrow(GetWorld(), CameraLocation, LineEnd, 10, FLinearColor::Black, 0.0f, 1);
		}

		{
			ULocalPlayer* LocalPlayer = Player->GetWorld()->GetFirstLocalPlayerFromController();
			if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport)
			{
				FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
					LocalPlayer->ViewportClient->Viewport,
					Player->GetWorld()->Scene,
					LocalPlayer->ViewportClient->EngineShowFlags)
					.SetRealtimeUpdate(true));

				FVector ViewLocation;
				FRotator ViewRotation;
				SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
			}
		}
	}

	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("TickLodSubsystem::Tick"))
	ParallelFor(AgentComponents.Num(), 
		[this](int32 InIndex)
	{
		if (AgentComponents[InIndex])
		{
			if (ACharacter* Agent = Cast<ACharacter>(AgentComponents[InIndex]->GetOwner()))
			{
				FVector AgentLocation = Agent->GetActorLocation();

				if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
				{
					UCameraComponent* Camera = Cast<UCameraComponent>(Player->GetComponentByClass(UCameraComponent::StaticClass()));
					if (Camera)
					{
						FVector CameraLocation = Camera->GetComponentToWorld().GetLocation();
						FVector CameraForwardTemp = Camera->GetComponentToWorld().GetRotation().Vector();

						FVector LineEnd = CameraLocation + (CameraForwardTemp * 300.f);
						LineEnd.Z = CameraLocation.Z;
						FVector CameraForward = FVector(LineEnd - CameraLocation).GetSafeNormal2D();

						FVector DeltaLocation = CameraLocation - AgentLocation;

						//bool bIsInFrustum = IsInFrustum(Agent);
						//if (!bIsInFrustum)
						//bool IsRendered = Agent->WasRecentlyRendered(GetWorld()->GetDeltaSeconds());
						//if(!IsRendered)
						float dot = FVector::DotProduct(CameraForward, DeltaLocation.GetSafeNormal2D());
						if (dot > 0.f)
						{
							// ¼û±è
							Mutex.Lock();
							// ÀÌµ¿ Æ½ ÁÙÀÌ±â
							Agent->GetMovementComponent()->SetComponentTickEnabled(false);
							// ¿¡´Ï¸ÞÀÌ¼Ç Æ½ ÁÙÀÌ±â
							Agent->GetMesh()->SetComponentTickEnabled(false);

							Agent->SetActorHiddenInGame(true);
							Mutex.Unlock();
						}
						else
						{
							// º¸¿©Áü
							Mutex.Lock();
							Agent->GetMovementComponent()->SetComponentTickEnabled(true);
							Agent->GetMesh()->SetComponentTickEnabled(true);
							Agent->SetActorHiddenInGame(false);
							Mutex.Unlock();
						}

						TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("TickLodSubsystem::LineTraceSingleByChannel"))
						const ECollisionChannel CollisionChannel = Player->GetRootComponent()->GetCollisionObjectType();
						FVector TraceStart = CameraLocation + FVector(0.f, 0.f, 20.f);
						FVector TraceEnd = CameraLocation - FVector(0.f, 0.f, 20.f);
						FHitResult FloorHit;
						bool bFindFloor = GetWorld()->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, CollisionChannel);
						if (bFindFloor)
						{
							FloorHit.Location;
							int iwer = 0;
							int iwererw = iwer;
						}
					}
				}
			}
		}
	});
}

bool UTickLodSubsystem::IsInFrustum(AActor* Actor)
{
	if (SceneView != nullptr)
	{
		return SceneView->ViewFrustum.IntersectSphere(Actor->GetActorLocation(), Actor->GetSimpleCollisionRadius()*2);
	}
	return false;
}

void UTickLodSubsystem::RegisterAgent(USFTickLodComponent* Component)
{
	check(Component->Index == INDEX_NONE);

	Component->Index = AgentComponents.Num();
	AgentComponents.Add(Component);
}

#pragma endregion UTickLodSubsystem