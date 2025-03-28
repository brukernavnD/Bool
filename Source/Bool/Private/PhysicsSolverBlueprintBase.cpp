#include "PhysicsSolverBlueprintBase.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"


APhysicsSolverBlueprintBase::APhysicsSolverBlueprintBase()
{
}

FVector APhysicsSolverBlueprintBase::WallCollisionSetExitDirection_Implementation( ABallActor* UnModifiedBall, const FHitResult& Hit)
{
	return FVector::ZeroVector;
}

float APhysicsSolverBlueprintBase::WallCollisionSetExitSpeed_Implementation(ABallActor* UnModifiedBall, const FHitResult& Hit)
{
	return 1;
}

FRotator APhysicsSolverBlueprintBase::WallCollisionSetAngularExitDirection_Implementation(ABallActor* UnModifiedBall, const FHitResult& Hit)
{
	return FRotator::ZeroRotator;
}

float APhysicsSolverBlueprintBase::WallCollisionSetAngularExitSpeed_Implementation(ABallActor* UnModifiedBall, const FHitResult& Hit)
{
	return 1;
}

FVector APhysicsSolverBlueprintBase::BallCollisionSetExitDirection_Implementation(ABallActor* UnModifiedBall, const TArray<ABallActor*>& OtherBalls, const FHitResult& Hit)
{
	return FVector::ZeroVector;
}

float APhysicsSolverBlueprintBase::BallCollisionSetExitSpeed_Implementation(ABallActor* UnModifiedBall, const TArray<ABallActor*>& OtherBalls, FVector OutDirection, const FHitResult& Hit)
{
	return 1;
}

FRotator APhysicsSolverBlueprintBase::BallCollisionSetAngularExitDirection_Implementation(ABallActor* UnModifiedBall, const TArray<ABallActor*>& OtherBalls, const FHitResult& Hit)
{
	return FRotator::ZeroRotator;
}

float APhysicsSolverBlueprintBase::BallCollisionSetAngularExitSpeed_Implementation(ABallActor* UnModifiedBall, const TArray<ABallActor*>& OtherBalls, FRotator OutDirection, const FHitResult& Hit)
{
	return 1;
}

FVector APhysicsSolverBlueprintBase::OtherBallCollisionSetExitDirection_Implementation(const TArray<ABallActor*>& OtherBalls, ABallActor* UnModifiedBall, ABallActor* InitialBall, const FHitResult& Hit)
{
	return FVector::ZeroVector;
}

float APhysicsSolverBlueprintBase::OtherBallCollisionSetExitSpeed_Implementation(const TArray<ABallActor*>& OtherBalls, ABallActor* UnModifiedBall, ABallActor* InitialBall, FVector OutDirection, const FHitResult& Hit)
{
	return 1;
}

FRotator APhysicsSolverBlueprintBase::OtherBallCollisionSetAngularExitDirection_Implementation(const TArray<ABallActor*>& OtherBalls, ABallActor* UnModifiedBall, ABallActor* InitialBall, const FHitResult& Hit)
{
	return FRotator::ZeroRotator;
}

float APhysicsSolverBlueprintBase::OtherBallCollisionSetAngularExitSpeed_Implementation(const TArray<ABallActor*>& OtherBalls, ABallActor* UnModifiedBall, ABallActor* InitialBall, FRotator OutDirection, const FHitResult& Hit)
{
	return 1;
}

void APhysicsSolverBlueprintBase::UpdateBallVelocity_Implementation(ABallActor* BallActor, float DeltaTime)
{
	
}

void APhysicsSolverBlueprintBase::UpdateBallAngularVelocity_Implementation(ABallActor* BallActor, float DeltaTime)
{
	
}

void APhysicsSolverBlueprintBase::PerformMovement_Implementation(ABallActor* BallActor, float DeltaTime)
{
	//move the component
	BallActor->SphereComponent->MoveComponent(BallActor->GetBallVelocity() * DeltaTime, BallActor->SphereComponent->GetComponentRotation() + BallActor->GetBallAngularVelocity() * DeltaTime, true);
}

void APhysicsSolverBlueprintBase::ThisDrawDebugSphere(AActor* WorldContextObject, const FVector& Location, float Radius, int32 Segments, const FColor& Colour, bool PersistentLines, float LifeTime)
{
	DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, Segments, Colour, PersistentLines, LifeTime);
}

void APhysicsSolverBlueprintBase::ThisDrawDebugDirectionalArrow(AActor* WorldContextObject, const FVector& Start, const FVector& End, float Length, const FColor& Colour, bool PersistentLines, float LifeTime, int32 DepthPriority, float Thickness)
{
	DrawDebugDirectionalArrow(WorldContextObject->GetWorld(), Start, End, Length, Colour, PersistentLines, LifeTime, DepthPriority, Thickness);
}
