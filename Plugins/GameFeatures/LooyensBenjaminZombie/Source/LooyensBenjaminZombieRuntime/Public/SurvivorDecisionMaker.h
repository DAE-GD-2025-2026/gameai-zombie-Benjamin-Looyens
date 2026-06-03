// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivorDecisionMaker.generated.h"

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;

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
	UInventoryComponent* m_pInventory = nullptr;
	UHealthComponent* m_pHealth = nullptr;
	UStaminaComponent* m_pStamina = nullptr;
};
