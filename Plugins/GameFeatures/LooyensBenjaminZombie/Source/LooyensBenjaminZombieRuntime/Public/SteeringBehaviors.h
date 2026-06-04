#pragma once

#include "SteeringHelpers.h"
#include "Kismet/KismetMathLibrary.h"

class ASurvivorPawn;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) = 0;
	virtual void DebugRender(const ASurvivorPawn& Survivor);

	void SetTarget(const FTargetData& NewTarget) { Target = NewTarget; }
	
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

	static void ApplySteering(ASurvivorPawn* pSurvivor, const SteeringOutput& steering, bool autoOrient = true);

protected:
	FTargetData Target;
};

// Seek Behavior
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;
};

// Flee Behavior
class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;
};

// Arrive Behavior
class Arrive : public Seek
{
public:
	Arrive() = default;
	virtual ~Arrive() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;
	virtual void SetTargetRadius(float newRadius) { m_StopRadius = newRadius; };

protected:
	float m_MaxSpeed = 600.0f; // Default ACharacter speed
	float m_SlowRadius = 1000.0f;
	float m_StopRadius = 150.0f;
};

// Face Behavior
class Face : public ISteeringBehavior
{
public:
	Face() = default;
	virtual ~Face() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;
};

// Pursuit Behavior
class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;

private:
	FVector2D m_PredictedPos = FVector2D::UnitVector;
};

// Evade Behavior
class Evade : public Pursuit
{
public:
	Evade() = default;
	virtual ~Evade() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;
	void SetEvadeRadius(float newRadius) { m_EvasionRadius = newRadius; };
	// MAYB : Perhaps store a color inside pursuit as protected, and when creating evade, change that color so that in the debug render its not the same

private:
	float m_EvasionRadius = 700.0f;
};

// Wander Behavior
class Wander : public ISteeringBehavior
{
public:
	Wander() = default;
	virtual ~Wander() override = default;

	virtual SteeringOutput CalculateSteering(float DeltaT, const ASurvivorPawn& Survivor) override;
	virtual void DebugRender(const ASurvivorPawn& Survivor) override;

protected:
	float m_OffsetDistance = 200.0f;
	float m_Radius = 100.0f;
	float m_MaxChange = FMath::DegreesToRadians(15);
	float m_CurrentAngle = 0.0f;

	// Debug Render
	FVector2D m_WanderPos = FVector2D::UnitVector;
};