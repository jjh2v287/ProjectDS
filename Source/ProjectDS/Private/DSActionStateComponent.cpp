// Fill out your copyright notice in the Description page of Project Settings.


#include "DSActionStateComponent.h"
#include "DSActionStateDataAsset.h"
#include "DSActionState.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UDSActionStateComponent::UDSActionStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDSActionStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDSActionStateComponent::InitPreset(TWeakObjectPtr<UDSActionStateDataAsset> ActionStateDataAsset, ACharacter* Character)
{
	OwnerCharacter = Character;

	if (ActionStateDataAsset->IdleTag.IsValid() && ActionStateDataAsset->IdleAction->IsValidLowLevel())
	{
		IdleTag = ActionStateDataAsset->IdleTag;
		//UDSActionState* Action = NewObject<UDSActionState>(OwnerCharacter, ActionStateDataAsset->IdleAction->GetFName(), RF_StrongRefOnFrame, ActionStateDataAsset->IdleAction->GetDefaultObject(), true);
		UDSActionState* Action = Cast<UDSActionState>(UGameplayStatics::SpawnObject(ActionStateDataAsset->IdleAction, OwnerCharacter));
		ActionStates.Add(ActionStateDataAsset->IdleTag, Action);
		ActiveActionStates = Action;
		ActiveActionStates->OnBegine();
	}

	for (auto itr : ActionStateDataAsset->ActionState)
	{
		//UDSActionState* Action = NewObject<UDSActionState>(OwnerCharacter->GetWorld(), itr.Value->GetFName(), RF_StrongRefOnFrame, itr.Value->GetDefaultObject(), true);
		UDSActionState* Action = Cast<UDSActionState>(UGameplayStatics::SpawnObject(itr.Value, OwnerCharacter));
		ActionStates.Add(itr.Key, Action);
	}
}

// Called every frame
void UDSActionStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (!ActiveActionStates)
		return;

	ActiveActionStates->OnTick();
}
