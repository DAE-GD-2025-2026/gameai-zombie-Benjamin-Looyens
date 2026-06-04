// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"

// Base Project Includes
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Zombies/BaseZombie.h>

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
			if (ASurvivorPawn* pSurvivor = dynamic_cast<ASurvivorPawn*>(GetOwner())) {
				// TODO : maybe can store this in memory
				const double dist = FVector::Distance(pSurvivor->GetActorLocation(), pHouse->GetActorLocation());

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Distance between house and survivor: %f"), dist)); 
			}

			m_pDecisionMaker->AddHouseMemory(pHouse);
		}
	}
}
