// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"

// Base Project Includes
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Zombies/BaseZombie.h>
#include <GameAI_Zombie/PurgeZones/PurgeZone.h>

#include "Utils/ItemsInclude.h"

#include "Public/SurvivorDecisionMaker.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	const auto& pOwner = GetOwner();

	if (auto PerceptionComp = pOwner->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Perception Setup Success")));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Perception Setup Failure")));
	}

	// Add Further Components
	UActorComponent* pBaseDecisionMaker = pOwner->AddComponentByClass(USurvivorDecisionMaker::StaticClass(), false, FTransform::Identity, true);
	m_pDecisionMaker = Cast<USurvivorDecisionMaker>(pBaseDecisionMaker);
	if (m_pDecisionMaker) {
		m_pDecisionMaker->Init();
		m_pDecisionMaker->SetComponentTickEnabled(true);
	}
	pOwner->FinishAddComponent(pBaseDecisionMaker, false, FTransform::Identity);
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// New "Stimuli"
	if (Stimulus.WasSuccessfullySensed()) { 
		if (AHouse* pHouse = Cast<AHouse>(Actor)) {
			m_pDecisionMaker->AddHouseMemory(pHouse);
		}

		if (APurgeZone* pPurge = Cast<APurgeZone>(Actor)) {
			m_pDecisionMaker->AddPurgeMemory(pPurge);
		}

		if (ABaseZombie* pZombie = Cast<ABaseZombie>(Actor)) {
			m_pDecisionMaker->AddZombieMemory(pZombie);
		}

		if (ABaseItem* pItem = Cast<ABaseItem>(Actor)) {
			m_pDecisionMaker->AddItemMemory(pItem);
		}
	}
}
