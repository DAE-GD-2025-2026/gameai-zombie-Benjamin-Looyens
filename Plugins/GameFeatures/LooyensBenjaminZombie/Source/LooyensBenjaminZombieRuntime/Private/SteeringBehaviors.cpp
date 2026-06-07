#include "SteeringBehaviors.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>

// General
// HACK : Debug rendering does Survivor.Z -85.0 since the mesh bottom is not the Actor position, but is not ideal

// Base Debug Render
void ISteeringBehavior::DebugRender(const ASurvivorPawn& Survivor)
{
	constexpr FColor CUR_VELOCITY_COLOR{ 0, 180, 255 };

	UWorld* pWorld = Survivor.GetWorld();

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	FVector2D curVel = SteeringPawnAccess::GetPawnLinearVelocity(Survivor);
	FVector2D curPos = SteeringPawnAccess::GetPawnPosition(Survivor);
	DrawDebugLine(pWorld, FVector{ curPos, SurvivorZ }, FVector{ curPos + curVel, SurvivorZ }, CUR_VELOCITY_COLOR);
}

void ISteeringBehavior::ApplySteering(ASurvivorPawn* pSurvivor, const SteeringOutput& steering, bool autoOrient)
{
	pSurvivor->AddMovementInput(FVector{ steering.LinearVelocity, 0.f });
	
	// MAYB : Debug Render?
	if (autoOrient) {
		const auto& fullVelocity = pSurvivor->GetVelocity();

		if (!fullVelocity.IsNearlyZero()) { // Ensure no weird calculations with float errors
			const FRotator newRotation = fullVelocity.ToOrientationRotator();

			pSurvivor->SetActorRotation(newRotation); // TODO : apply some sort of MaxAngularSpeed
		}
	}
	else pSurvivor->AddActorLocalRotation(FRotator(0.0f, (steering.AngularVelocity), 0.0f));
}

// Seek Behavior
SteeringOutput Seek::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering{};

	steering.LinearVelocity = Target.Position - SteeringPawnAccess::GetPawnPosition(Survivor); // No need to normalize, as the AddMovementInput() fuction within the SteeringSurvivor::Tick() will normalize

	steering.LinearVelocity.Normalize();
	return steering;
}

void Seek::DebugRender(const ASurvivorPawn& Survivor)
{
	ISteeringBehavior::DebugRender(Survivor);
	

	constexpr FColor TO_TARGET_COLOR{ 255, 0, 0 };

	UWorld* pWorld = Survivor.GetWorld();

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	// Line Towards Target
	DrawDebugLine(pWorld, FVector{ SteeringPawnAccess::GetPawnPosition(Survivor), SurvivorZ }, FVector{Target.Position, SurvivorZ }, TO_TARGET_COLOR);
}

// Flee Behavior
SteeringOutput Flee::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering = Seek::CalculateSteering(DeltaT, Survivor);

	steering.LinearVelocity = -steering.LinearVelocity;

	return steering;
}

void Flee::DebugRender(const ASurvivorPawn& Survivor)
{
	Seek::DebugRender(Survivor);

	constexpr FColor AWAY_FROM_TARGET_COLOR{ 0, 255, 0 };

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	const FVector2D& SurvivorPos = SteeringPawnAccess::GetPawnPosition(Survivor);
	const FVector2D toTarget = Target.Position - SurvivorPos;
	FVector2D direction = -toTarget;
	direction.Normalize();
	direction *= SteeringPawnAccess::GetPawnMaxLinearSpeed(Survivor);

	UWorld* pWorld = Survivor.GetWorld();
	DrawDebugLine(pWorld, FVector{ SurvivorPos, SurvivorZ }, FVector{ SurvivorPos, SurvivorZ } + FVector{ direction, 0.0f}, AWAY_FROM_TARGET_COLOR);
}

// Arrive Behavior
SteeringOutput Arrive::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	const float distance = FVector2D::Distance(SteeringPawnAccess::GetPawnPosition(Survivor), Target.Position);
	float speed = m_MaxSpeed;

	if (distance < m_SlowRadius) {
		const float lerpFactor = (distance - m_StopRadius) / (m_SlowRadius - m_StopRadius);
		speed = FMath::Lerp(0.0f, m_MaxSpeed, lerpFactor);
	}
	else if (distance < m_StopRadius) return SteeringOutput{}; // Within stop radius, do not move
	
	// TEMP : If I use arrive, I NEED to fix this:
	//Survivor.SetMaxLinearSpeed(speed);

	return Seek::CalculateSteering(DeltaT, Survivor); // Same exact movement (direction) behavior as seek
}

void Arrive::DebugRender(const ASurvivorPawn& Survivor)
{
	Seek::DebugRender(Survivor);

	constexpr FColor SLOW_COLOR{ 0, 0, 255 };
	constexpr FColor STOP_COLOR{ 0, 0, 150 };

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	UWorld* pWorld = Survivor.GetWorld();
	// Had to pass the default variables, since I needed to access the X and Y axes
	DrawDebugCircle(pWorld, FVector{ SteeringPawnAccess::GetPawnPosition(Survivor), SurvivorZ }, m_SlowRadius, 15, SLOW_COLOR, false, (-1.0f), (uint8)0U, (0.0F), { 1, 0, 0 }, { 0, 1, 0 }, false);
	DrawDebugCircle(pWorld, FVector{ SteeringPawnAccess::GetPawnPosition(Survivor), SurvivorZ }, m_StopRadius, 15, STOP_COLOR, false, (-1.0f), (uint8)0U, (0.0F), { 1, 0, 0 }, { 0, 1, 0 }, false);
}

// Face Behavior
SteeringOutput Face::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering{};

	const FVector2D curPos = SteeringPawnAccess::GetPawnPosition(Survivor);

	const float curRot = SteeringPawnAccess::GetPawnRotation(Survivor);
	const float goalRot = FMath::Atan2(Target.Position.Y - curPos.Y, Target.Position.X - curPos.X);
	//const float maxRot = Survivor.GetMaxAngularSpeed() * DeltaT;
	const float maxRot = 360.0f * DeltaT; // TEMP : Pawns do not have MaxAngularSpeed, so have to put raw value here
	const float difRot = FMath::FindDeltaAngleDegrees(curRot, FMath::RadiansToDegrees(goalRot)); // AddActorLocalRotation expects degrees

	steering.AngularVelocity = FMath::Clamp(difRot, -maxRot, maxRot);

	return steering;
}

void Face::DebugRender(const ASurvivorPawn& Survivor)
{
}

// Pursuit Behavior
SteeringOutput Pursuit::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering{};

	const float estimatedTime = FVector2D::Distance(SteeringPawnAccess::GetPawnPosition(Survivor), Target.Position) / SteeringPawnAccess::GetPawnMaxLinearSpeed(Survivor);
	m_PredictedPos = Target.Position + (Target.LinearVelocity * estimatedTime); // Store in class for Debug Render

	steering.LinearVelocity = m_PredictedPos - SteeringPawnAccess::GetPawnPosition(Survivor);

	steering.LinearVelocity.Normalize();
	return steering;
}

void Pursuit::DebugRender(const ASurvivorPawn& Survivor)
{
	ISteeringBehavior::DebugRender(Survivor);

	constexpr float PREDICTED_POS_POINT_SIZE = 10.0f;
	constexpr FColor PREDICTED_POS_COLOR{ 100, 100, 200 };

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	UWorld* pWorld = Survivor.GetWorld();

	DrawDebugPoint(pWorld, FVector{ m_PredictedPos, SurvivorZ }, PREDICTED_POS_POINT_SIZE, PREDICTED_POS_COLOR);
}

// Evade Behavior
SteeringOutput Evade::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering = Pursuit::CalculateSteering(DeltaT, Survivor);

	steering.LinearVelocity = -steering.LinearVelocity;

	if (FVector2D::DistSquared(SteeringPawnAccess::GetPawnPosition(Survivor), Target.Position) >= FMath::Square(m_EvasionRadius)) steering.IsValid = false;

	return steering;
}

void Evade::DebugRender(const ASurvivorPawn& Survivor)
{
	Pursuit::DebugRender(Survivor);

	constexpr FColor EVASION_RADIUS_COLOR{ 150, 0, 0 };

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	UWorld* pWorld = Survivor.GetWorld();
	// Had to pass the default variables, since I needed to access the X and Y axes
	DrawDebugCircle(pWorld, FVector{ SteeringPawnAccess::GetPawnPosition(Survivor), SurvivorZ }, m_EvasionRadius, 15, EVASION_RADIUS_COLOR, false, (-1.0f), (uint8)0U, (0.0F), { 1, 0, 0 }, { 0, 1, 0 }, false);
}

// Wander Behavior
SteeringOutput Wander::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering{};

	const float angleChange = std::clamp((FMath::FRand() * 2.0f) - 1.0f, -m_MaxChange, m_MaxChange);
	m_CurrentAngle += angleChange;

	const FVector forward = Survivor.GetActorForwardVector() * m_OffsetDistance;

	m_WanderPos = { FMath::Sin(m_CurrentAngle), FMath::Cos(m_CurrentAngle) };
	m_WanderPos *= m_Radius;
	m_WanderPos += SteeringPawnAccess::GetPawnPosition(Survivor) + FVector2D{ forward.X, forward.Y };

	steering.LinearVelocity = m_WanderPos - SteeringPawnAccess::GetPawnPosition(Survivor);

	steering.LinearVelocity.Normalize();
	return steering;
}

void Wander::DebugRender(const ASurvivorPawn& Survivor)
{
	ISteeringBehavior::DebugRender(Survivor);

	constexpr float WANDER_POINT_SIZE = 10.0f;
	constexpr FColor WANDER_POINT_COLOR{ 0, 0, 200 };
	constexpr FColor WANDER_CIRCLE_COLOR{ 0, 0, 100 };

	const double SurvivorZ = Survivor.GetActorLocation().Z - 85.0;

	UWorld* pWorld = Survivor.GetWorld();

	// Circle
	DrawDebugCircle(pWorld, FVector{ SteeringPawnAccess::GetPawnPosition(Survivor), SurvivorZ } + Survivor.GetActorForwardVector() * m_OffsetDistance,
					m_Radius, 15, WANDER_CIRCLE_COLOR, false, (-1.0f), (uint8)0U, (0.0F), { 1, 0, 0 }, { 0, 1, 0 }, false);

	// Point on Circle
	DrawDebugPoint(pWorld, FVector{ m_WanderPos, SurvivorZ + 0.1f }, WANDER_POINT_SIZE, WANDER_POINT_COLOR); // + 0.1f to ensure it is above the radius circle
}

PathFollow::PathFollow()
{
	pSeek = MakeUnique<Seek>();
	pArrive = MakeUnique<Arrive>();
	pArrive->SetTargetRadius(10.0f);
}

void PathFollow::SetPath(const TArray<FVector2D>& path)
{
	pathArr = path;

	currentPathIndex = -1;
	GotoNextPathPoint();
}

void PathFollow::SetPath(const TArray<FVector>& path)
{
	pathArr.Reset(path.Num());

	Algo::Transform(path, pathArr,
		[](const FVector& vec) {
			return FVector2D{ vec.X, vec.Y };
		}
	);

	currentPathIndex = -1;
	GotoNextPathPoint();
}

SteeringOutput PathFollow::CalculateSteering(float DeltaTime, const ASurvivorPawn& Survivor)
{
	if (currentPathIndex < static_cast<int>(pathArr.Num()))
	{
		float agentRadius = Survivor.GetSimpleCollisionRadius() * 2; // Not sure if GetSimpleCollisionRadius is equivalent to GetCapsuleRadius
		FVector2D ToPathPoint{ pathArr[currentPathIndex] - SteeringPawnAccess::GetPawnPosition(Survivor) };

		if (ToPathPoint.SizeSquared() < agentRadius * agentRadius)
		{
			//Reached point of the path
			GotoNextPathPoint();
		}
	}

	if (pCurrentSteering != nullptr)
	{
		return pCurrentSteering->CalculateSteering(DeltaTime, Survivor);
	}
	return SteeringOutput{};
}

bool PathFollow::HasFinishedPath(const ASurvivorPawn& Survivor) const
{
	if (pathArr.IsEmpty()) return true;
	if (currentPathIndex >= static_cast<int>(pathArr.Num())) {
		const float agentRadius = Survivor.GetSimpleCollisionRadius() * 2; // Not sure if GetSimpleCollisionRadius is equivalent to GetCapsuleRadius
		const FVector2D ToPathPoint{ pathArr.Last() - SteeringPawnAccess::GetPawnPosition(Survivor)};
		return ToPathPoint.SizeSquared() < agentRadius * agentRadius;
	}

	return false;
}

void PathFollow::GotoNextPathPoint()
{
	++currentPathIndex;
	if (currentPathIndex >= static_cast<int>(pathArr.Num())) return;

	if (currentPathIndex == pathArr.Num() - 1)
	{
		FTargetData PathTarget{ pathArr[currentPathIndex] };
		//We have reached the last node
		pArrive->SetTarget(PathTarget);
		pCurrentSteering = pArrive.Get();
	}
	else
	{
		FTargetData PathTarget{ pathArr[currentPathIndex] };
		//Move to the next node
		pSeek->SetTarget(PathTarget);
		pCurrentSteering = pSeek.Get();
	}
}

// BLENDED
BlendedSteering::BlendedSteering(const TArray<WeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{}

SteeringOutput BlendedSteering::CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor)
{
	SteeringOutput steering{};

	const float totalWeight = Algo::Accumulate(WeightedBehaviors, 0.0f,
		[](float prev, const WeightedBehavior& behavior) { return prev + behavior.Weight; });
	if (totalWeight <= 0.0f) return steering;

	for (const auto& behavior : WeightedBehaviors) {
		const float curWeight = (behavior.Weight / totalWeight);
		const SteeringOutput curBehavior = behavior.pBehavior->CalculateSteering(DeltaT, Survivor);

		steering.LinearVelocity += curWeight * curBehavior.LinearVelocity;
		steering.AngularVelocity += curWeight * curBehavior.AngularVelocity;
		steering.IsValid = steering.IsValid && curBehavior.IsValid;
	}

	steering.LinearVelocity.Normalize();
	return steering;
}

void BlendedSteering::SetTargetAllBlends(const FTargetData& target)
{
	SetTarget(target);

	for (auto& behavior : WeightedBehaviors) {
		behavior.pBehavior->SetTarget(target);
	}
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = Algo::FindByPredicate(WeightedBehaviors, 
		[SteeringBehavior](const WeightedBehavior& Elem) {
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if (it) {
		return &it->Weight;
	}

	return nullptr;
}
