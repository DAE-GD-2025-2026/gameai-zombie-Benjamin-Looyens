// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorDecisionMaker.h"

#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>

#include "WanderAction.h"

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
	m_Memory.pSurvivor = Cast<ASurvivorPawn>(GetOwner());
	m_Memory.pInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	m_Memory.pHealth = GetOwner()->GetComponentByClass<UHealthComponent>();
	m_Memory.pStamina = GetOwner()->GetComponentByClass<UStaminaComponent>();

	if (m_Memory.pInventory) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to inventory")));
	}
	if (m_Memory.pHealth) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to health")));
	}
	if (m_Memory.pStamina) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Got reference to stamina")));
	}

	// Create Utility Actions
	m_Actions.Add(MakeUnique<WanderAction>());
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

	if (m_Actions.Num() <= 0) return; // No actions to loop through

	std::pair<int, float> bestAction{ 0, -50.0f };
	for (size_t index{}; index < m_Actions.Num(); index++) {
		const auto& pCurAction = m_Actions[index];
		const float curValue = pCurAction->Evaluate(m_Memory);

		if (curValue > bestAction.second) {
			bestAction.first = index;
			bestAction.second = curValue;
		}
	}

	m_Actions[bestAction.first]->Execute(m_Memory);
}

