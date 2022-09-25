// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavigationData.h"
#include "ActorSpawn.generated.h"

UCLASS()
class PROJECTDS_API AActorSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorSpawn();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ActorSpawn)
	TSubclassOf<ACharacter> SpawnTargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ActorSpawn)
	int SpawnCellX = 32;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ActorSpawn)
	int SpawnCellY = 32;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ActorSpawn)
	float SpawnCellSize = 200.f;

	UPROPERTY()
	TWeakObjectPtr<UWorld> World;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ProcessSpawn();
	const ANavigationData* FindNavigationDataForActor();
};
