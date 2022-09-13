// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFTickLodComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTDS_API USFTickLodComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USFTickLodComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(Transient)
	int32 Index = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TickOpt")
	TArray<float> FramesSkippedScreenSizeThresholds = { 0.5f, 0.35f };
};

UCLASS()
class PROJECTDS_API UTickLodSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<USFTickLodComponent*> AgentComponents;

	FSceneView* SceneView;

	FCriticalSection Mutex;

public:
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;
	
	bool IsInFrustum(AActor* Actor);

	void RegisterAgent(USFTickLodComponent* Component);
};