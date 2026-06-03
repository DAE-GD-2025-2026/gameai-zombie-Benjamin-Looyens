// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"

// Base Project Includes
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>

//#include "Utils/ItemsInclude.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Perception Setup Success")));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Perception Setup Failure")));
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Saw Something!")));

	if (AHouse* pHouse = Cast<AHouse>(Actor)) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("Saw a House!")));

		if (ASurvivorPawn* pSurvivor = dynamic_cast<ASurvivorPawn*>(GetOwner())) {
			const double dist = FVector::Distance(pSurvivor->GetActorLocation(), pHouse->GetActorLocation());

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Distance between house and survivor: %f"), dist));
		}
	}
}
