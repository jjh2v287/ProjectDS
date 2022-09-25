// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Algo/RemoveIf.h"
#include "GameFramework/Character.h"

// Sets default values
AActorSpawn::AActorSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActorSpawn::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	ProcessSpawn();
}

// Called every frame
void AActorSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AActorSpawn::ProcessSpawn()
{
	if (!World.Get())
		return;

	int MaxSpawnCount = SpawnCellX * SpawnCellY;
	for (int XCount = 0; XCount < SpawnCellX; XCount++)
	{
		float sX = -(((SpawnCellSize * SpawnCellX)) / 2) + (SpawnCellSize * XCount);
		for (int YCount = 0; YCount < SpawnCellY; YCount++)
		{
			float sY = -(((SpawnCellSize * SpawnCellY)) / 2) + (SpawnCellSize * YCount);
			ACharacter* Character = World->SpawnActorDeferred<ACharacter>(SpawnTargetClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			FTransform NewTransform;
			FVector NewLocation = FVector::ZeroVector;
			NewLocation.X = sX;
			NewLocation.Y = sY;

			{
				const ANavigationData* NavData = FindNavigationDataForActor();
				if (NavData)
				{
					const ANavigationData& refNavData = *NavData;
					FSharedConstNavQueryFilter NavigationFilter;
					FNavLocation OutLocation;
					const FVector ProjectionExtent(0.0f, 0.0f, 1024.0f);
					refNavData.ProjectPoint(NewLocation, OutLocation, ProjectionExtent, NavigationFilter);
					NewLocation = OutLocation.Location;
					NewLocation.Z += 88.0f;
				}
			}

			NewTransform.SetLocation(NewLocation);
			UGameplayStatics::FinishSpawningActor(Character, NewTransform);
			/*
				AI NPC 또는 몬스터 스폰시 AIController이 정상적으로 붙지않을때 확인.
				AutoPossessAI EAutoPossessAI::PlacedInWorldOrSpawned
			*/
		}
	}
}

const ANavigationData* AActorSpawn::FindNavigationDataForActor()
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World.Get());

	if (NavSys == nullptr)
	{
		return nullptr;
	}

	// try to match navigation agent for querier
	INavAgentInterface* NavAgent = GetOwner() ? Cast<INavAgentInterface>(GetOwner()) : NULL;
	if (NavAgent)
	{
		const FNavAgentProperties& NavAgentProps = NavAgent->GetNavAgentPropertiesRef();
		return NavSys->GetNavDataForProps(NavAgentProps, NavAgent->GetNavAgentLocation());
	}

	return NavSys->GetDefaultNavDataInstance();
}