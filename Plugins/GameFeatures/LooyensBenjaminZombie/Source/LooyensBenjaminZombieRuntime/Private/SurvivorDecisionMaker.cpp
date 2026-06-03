// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorDecisionMaker.h"

#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>

// Sets default values for this component's properties
USurvivorDecisionMaker::USurvivorDecisionMaker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// ...
}

// HACK : Separate Init Function is weird but BeginPlay() would never be called
void USurvivorDecisionMaker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT("Added decision maker")));

	// Get Survivor Components
	m_pInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	m_pHealth = GetOwner()->GetComponentByClass<UHealthComponent>();
	m_pStamina = GetOwner()->GetComponentByClass<UStaminaComponent>();

	if (m_pInventory) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to inventory")));
	}
	if (m_pHealth) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to health")));
	}
	if (m_pStamina) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to stamina")));
	}
}

// Called when the game starts
void USurvivorDecisionMaker::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USurvivorDecisionMaker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto& pOwner = GetOwner();
	const auto& pos = pOwner->GetActorLocation();

	// TEMP : Location modification to test if works
	pOwner->SetActorLocation(pos + FVector{ 10, 0, 0 });
}

