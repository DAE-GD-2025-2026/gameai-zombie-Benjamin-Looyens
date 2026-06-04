// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorUtilityAction.h"

#include "SurvivorDecisionMaker.generated.h"

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;

struct SurvivorMemory
{
	// Add Memory of:
	// - Visible Zombies, Houses and Items
	// - Last seen Houses and Items
	// - Purge Zones

	ASurvivorPawn* pSurvivor = nullptr;
	UInventoryComponent* pInventory = nullptr;
	UHealthComponent* pHealth = nullptr;
	UStaminaComponent* pStamina = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LOOYENSBENJAMINZOMBIERUNTIME_API USurvivorDecisionMaker : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USurvivorDecisionMaker();

	void Init();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	SurvivorMemory m_Memory{};

	TArray<TUniquePtr<ISurvivorUtilityAction>> m_Actions{};
};
