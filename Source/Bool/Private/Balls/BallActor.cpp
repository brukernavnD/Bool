// Fill out your copyright notice in the Description page of Project Settings.


#include "Balls/BallActor.h"

#include "PhysicsDataBlueprint.h"
#include "Balls/BallUpgradeDataAsset.h"
#include "Bool/GoalActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/MathFwd.h"

//FMultiBodyCollisionGraph FMultiBodyCollisionGraph::BuildMultiBodyCollisionGraph(TArray<ABallActor*>& InBalls, UPhysicsDataBlueprint* PhysicsDataBP)
//{
//	//check if the array is empty or the physics data blueprint is invalid
//	if (InBalls.IsEmpty() || !PhysicsDataBP->IsValidLowLevelFast())
//	{
//		//return an empty graph
//		return FMultiBodyCollisionGraph();
//	}
//
//	//storage for the array of nodes
//	TArray<FMultiBodyCollisionGraphNode> LocNodes;
//
//	//iterate over the actors
//	for (ABallActor* BallActor : InBalls)
//	{
//		//create a new node
//		FMultiBodyCollisionGraphNode Node;
//
//		//set the ball actor of the node
//		Node.OurBall = BallActor;
//
//		//add the node to the nodes array
//		LocNodes.Add(Node);
//	}
//
//	//storage for balls that have incoming velocity
//	TArray<FMultiBodyCollisionGraphNode> IncomingVelocityBalls;
//
//	//storage for the balls that do not have incoming velocity
//	TArray<FMultiBodyCollisionGraphNode> NoIncomingVelocityBalls;
//
//	//iterate over the nodes
//	for (FMultiBodyCollisionGraphNode& Node : LocNodes)
//	{
//		//get the ball actor
//		ABallActor* BallActor = Node.OurBall.Get();
//
//		//check if the ball actor's velocity is nearly zero
//		if (BallActor->GetBallVelocity().IsNearlyZero())
//		{
//			//add it to the no incoming velocity balls
//			NoIncomingVelocityBalls.Add(Node);
//
//			//continue to the next iteration of the loop
//			continue;
//		}
//
//		//add it to the incoming velocity balls
//		IncomingVelocityBalls.Add(Node);
//	}
//
//	//create a new graph
//	FMultiBodyCollisionGraph Graph;
//
//	//set the nodes of the graph
//	Graph.Nodes = LocNodes;
//
//	//set the physics data blueprint of the graph
//	Graph.PhysicsDataBlueprint = PhysicsDataBP;
//
//	//storage for the maps of collision directions
//	TArray<TMap<ABallActor*, FVector>> CollisionDirections;
//
//	//iterate over the incoming velocity balls
//	for (FMultiBodyCollisionGraphNode& IncomingVelocityBall : IncomingVelocityBalls)
//	{
//		//storage for the collision directions
//		TMap<ABallActor*, FVector> CollisionDirectionsMap;
//
//		//get the collision directions from the ball
//		Graph.GetCollisionDirectionsRecursive(CollisionDirectionsMap, IncomingVelocityBall.OurBall.Get(), IncomingVelocityBall.OurBall->GetBallVelocity().GetSafeNormal());
//
//		//add the collision directions to the array
//		CollisionDirections.Add(CollisionDirectionsMap);
//	}
//
//	//return the graph
//	return Graph;
//}
//
//void FMultiBodyCollisionGraph::GetCollisionDirectionsRecursive(TMap<ABallActor*, FVector>& OutMap, ABallActor* InBallActor, FVector ImpartedVelDirection, const int Iterations)
//{
//	//check if we've reached the maximum number of iterations
//	if (Iterations >= 150)
//	{
//		//return early to prevent further execution
//		return;
//	}
//
//	//check if the ball actor is already in the out actors array
//	if (OutMap.Contains(InBallActor))
//	{
//		//return early to prevent further execution
//		return;
//	}
//
//	//iterate over the overlapping actors
//	for (AActor* OverlappingActor : InBallActor->OverlappingActors)
//	{
//		//check if the overlapping actor is not a ball actor
//		if (!OverlappingActor->IsA(ABallActor::StaticClass()))
//		{
//			//continue to the next iteration of the loop
//			continue;
//		}
//
//		//get the other ball actor
//		ABallActor* OverlappingBallActor = Cast<ABallActor>(OverlappingActor);
//
//		//check if the actor is already in the out actors array or is a goal actor
//		if (OutMap.Contains(OverlappingBallActor))
//		{
//			//continue to the next iteration of the loop
//			continue;
//		}
//
//		//get the distance the overlapping actor is from an infinite line originating from the ball actor with the direction of the imparted velocity
//		FVector ClosestPoint = UKismetMathLibrary::FindClosestPointOnLine(OverlappingBallActor->GetActorLocation(), InBallActor->GetActorLocation(), ImpartedVelDirection);
//
//		//check if the closest point is greater than the sum of the radii of the 2 balls
//		if (FVector::Dist(ClosestPoint, OverlappingBallActor->GetActorLocation()) > InBallActor->SphereComponent->GetScaledSphereRadius() + OverlappingBallActor->SphereComponent->GetScaledSphereRadius())
//		{
//			//continue to the next iteration of the loop
//			continue;
//		}
//
//		//get the hit normal
//		const FVector HitNormal = (OverlappingBallActor->GetActorLocation() - InBallActor->GetActorLocation()).GetSafeNormal();
//
//		//call the physics data blueprint to get the exit direction
//		const FVector ExitDirection = PhysicsDataBlueprint->MulitBallCollisionExitDir(InBallActor, OverlappingBallActor, ImpartedVelDirection, ClosestPoint, HitNormal);
//
//		//recurse
//		GetCollisionDirectionsRecursive(OutMap, OverlappingBallActor, ExitDirection, Iterations + 1);
//	}
//
//	//add the ball actor
//	OutMap.Add({InBallActor, ImpartedVelDirection});
//}

ABallActor::ABallActor()
{
	//enable ticking
	PrimaryActorTick.bCanEverTick = true;

	//create the component(s)
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Root"));
	//BallDetectionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BallDetection"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	//set the sphere component to be the root component
	SetRootComponent(SphereComponent);

	//setup attachment(s)
	//BallDetectionComponent->SetupAttachment(RootComponent);
	MeshComponent->SetupAttachment(RootComponent);

	//bind the OnHit and OnBeginOverlap events for the sphere component
	SphereComponent->OnComponentHit.AddDynamic(this, &ABallActor::OnSphereHit);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABallActor::OnSphereOverlap);

	//bind the OnBeginOverlap and OnEndOverlap events for the ball detection component
	//BallDetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABallActor::BallBeginDetectionOverlap);
	//BallDetectionComponent->OnComponentEndOverlap.AddDynamic(this, &ABallActor::BallEndDetectionOverlap);
}

void ABallActor::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get all actors with the ignore collision tag
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), ActorTagToIgnore, ActorsToIgnore);

	//iterate over the actors to ignore
	for (AActor* ActorToIgnore : ActorsToIgnore)
	{
		//ignore the actor
		SphereComponent->IgnoreActorWhenMoving(ActorToIgnore, true);
	}
}

void ABallActor::UpdateOldVelocities()
{
	//add the old velocity
	OldVelocities.Add(CurrentVelocity);

	//check if we have more than 10 old velocities
	if (OldVelocities.Num() > 10)
	{
		//remove the first element
		OldVelocities.RemoveAt(0);
	}
}

void ABallActor::Tick(const float DeltaTime)
{
	//call the parent implementation
	Super::Tick(DeltaTime);

	//update the old velocity
	UpdateOldVelocities();

	//move the component
	SphereComponent->MoveComponent(GetBallVelocity() * DeltaTime, SphereComponent->GetComponentRotation() + GetBallAngularVelocity().Rotation() * DeltaTime, true);
	
	////check if we're not using custom physics
	//if (!bUseCustomPhysics)
	//{
	//	//update the bool physics state
	//	UpdateBoolPhysicsState(DeltaTime);
	//
	//	//update the bool physics variables
	//	UpdatePhysicsVariables(DeltaTime);
	//
	//	//return early to prevent further execution
	//	return;
	//}
	//
	////check if we're not using custom collision response
	//if (!bUseCustomCollisionResponse)
	//{
	//	//return early to prevent further execution
	//	return;
	//}
	//
	////move the component
	//SphereComponent->MoveComponent(GetBallVelocity() * DeltaTime, SphereComponent->GetComponentRotation() + GetBallAngularVelocity().Rotation() * DeltaTime, true);
	//
	////update the bool physics state
	//UpdateBoolPhysicsState(DeltaTime);
	//
	////update the bool physics variables
	//UpdatePhysicsVariables(DeltaTime);
	//
	////check if we're outside the table
	//if (IsOutsideTable())
	//{
	//	//storage for the hit result of the trace
	//	FHitResult HitResult;
	//
	//	//do a sphere trace from the box position to the current location
	//	GetWorld()->SweepSingleByChannel(HitResult, BoxPosition, GetActorLocation(), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereComponent->GetScaledSphereRadius() + 1));
	//
	//	//check if we're not hitting anything
	//	if (!HitResult.IsValidBlockingHit())
	//	{
	//		//set the location to the box position
	//		SetActorLocation(BoxPosition);
	//	}
	//	else
	//	{
	//		//set the location to the hit result location
	//		SetActorLocation(HitResult.Location);
	//
	//		//process the wall hit
	//		ProcessWallHit(HitResult);
	//	}
	//}
}

void ABallActor::UpdateBoolPhysicsState(const float DeltaTime)
{
	//check if we're basically stationary
	if (GetBallVelocity().IsNearlyZero() && AngularVelocity.IsNearlyZero())
	{
		//set the physics state to stationary
		SetBoolPhysicsState(Ebps_Stationary);

		//check if we're in debug mode
		if (bDebugMode && bDisplayPhysicsState)
		{
			//draw a red debug sphere
			DrawDebugSphere(GetWorld(), GetActorLocation(), SphereComponent->GetScaledSphereRadius() + 1, 10, FColor::Red, false, 0);
		}

		//return to prevent further execution
		return;
	}

	//check if we're spinning
	if (DeltaTime <= 2 * SphereComponent->GetScaledSphereRadius() / (5 * TableSpinningFrictionCoefficient * (GetWorld()->GetGravityZ() / 100)) * AngularVelocity.Z)
	{
		//set the physics state to spinning
		SetBoolPhysicsState(Ebps_Spinning);

		//check if we're in debug mode
		if (bDebugMode && bDisplayPhysicsState)
		{
			//draw a blue debug sphere
			DrawDebugSphere(GetWorld(), GetActorLocation(), SphereComponent->GetScaledSphereRadius() + 1, 10, FColor::Blue, false, 0);
		}

		//return to prevent further execution
		return;
	}

	//check if we're sliding
	if (DeltaTime <= /*2 / 7 **/ InitialRelativeVelocity.Length() / TableSlidingFrictionCoefficient * (GetWorld()->GetGravityZ() / 100))
	{
		//set the physics state to sliding
		SetBoolPhysicsState(Ebps_Sliding);

		//check if we're in debug mode
		if (bDebugMode && bDisplayPhysicsState)
		{
			//draw a green debug sphere
			DrawDebugSphere(GetWorld(), GetActorLocation(), SphereComponent->GetScaledSphereRadius() + 1, 10, FColor::Green, false, 0);
		}

		//return to prevent further execution
		return;
	}

	//check if we're rolling
	if (DeltaTime <= CurrentVelocity.Length() * TableRollingFrictionCoefficient)
	{
		//set the physics state to rolling
		SetBoolPhysicsState(Ebps_Rolling);

		//check if we're in debug mode
		if (bDebugMode && bDisplayPhysicsState)
		{
			//draw a yellow debug sphere
			DrawDebugSphere(GetWorld(), GetActorLocation(), SphereComponent->GetScaledSphereRadius() + 1, 10, FColor::Yellow, false, 0);
		}

		//return to prevent further execution
		return;
	}

	//set the physics state to stationary
	SetBoolPhysicsState(Ebps_Stationary);
}

void ABallActor::UpdatePhysicsVariables(const float DeltaTime)
{
	//check the physics state
	switch (PhysicsState)
	{
		case Ebps_Spinning:
		{
			//set the angular velocity variable
			SetBallAngularVelocity(FVector(0, 0, (AngularVelocity.Z - 5 * TableSpinningFrictionCoefficient * (GetWorld()->GetGravityZ() / 100) / 2) * DeltaTime));

			//break
			break;
		}

		case Ebps_Rolling:
		{
			//get the linear velocity magnitude
			const float VelocityMagnitute = GetBallVelocity().Size() - TableRollingFrictionCoefficient * (-GetWorld()->GetGravityZ() / 100) * DeltaTime;

			//set the velocity
			SetBallVelocity(GetBallVelocity().GetSafeNormal() * VelocityMagnitute);

			//set the angular velocity variable
			SetBallAngularVelocity(FVector(0, GetBallVelocity().X / SphereComponent->GetScaledSphereRadius(), AngularVelocity.Z - 5 * TableRollingFrictionCoefficient * (-GetWorld()->GetGravityZ() / 100) * DeltaTime / (2 * SphereComponent->GetScaledSphereRadius())));

			//break
			break;
		}

		case Ebps_Sliding:
		{
			//storage for the world gravityz
			const float WorldGravityZ = -GetWorld()->GetGravityZ() / 100;

			//set the velocity
			SetBallVelocity(GetBallVelocity() - TableSlidingFrictionCoefficient * WorldGravityZ * DeltaTime * InitialRelativeVelocity.GetSafeNormal());

			//update the initial relative velocity
			InitialRelativeVelocity = GetBallVelocity() - 2 / 7 * TableSlidingFrictionCoefficient * WorldGravityZ * DeltaTime * InitialRelativeVelocity.GetSafeNormal();

			//set the angular velocity variable
			SetBallAngularVelocity(FVector(
				AngularVelocity.X - 5 * TableSlidingFrictionCoefficient * WorldGravityZ * DeltaTime / (2 * SphereComponent->GetScaledSphereRadius()),
				AngularVelocity.Y - 5 * TableSlidingFrictionCoefficient * WorldGravityZ * DeltaTime / (2 * SphereComponent->GetScaledSphereRadius()),
				AngularVelocity.Z - 5 * TableSpinningFrictionCoefficient * WorldGravityZ * DeltaTime / (2 * SphereComponent->GetScaledSphereRadius())));

			//break
			break;
		}
		default: ;
	}
}

void ABallActor::SetBoolPhysicsState(const TEnumAsByte<EBallPhysicsState> NewPhysicsState)
{
	//check if there is not a change in physics state
	if (PhysicsState == NewPhysicsState)
	{
		//return early to prevent further execution
		return;
	}

	//set the new bool physics state
	PhysicsState = NewPhysicsState;
}

bool ABallActor::IsOutsideTable() const
{
	return !UKismetMathLibrary::IsPointInBox(GetActorLocation(), BoxPosition, BoxSize);
}

//bool ABallActor::SolveStationaryBallMovingBallCollision(const FHitResult& Hit, const TObjectPtr<ABallActor> OtherBallActor)
//{
//	//storage for the old velocity
//	FVector OldVelocity = GetBallVelocity();
//
//	//check if we have any old velocities
//	if (OldVelocities.Num() > 0)
//	{
//		//set the old velocity to the last element in the array
//		OldVelocity = OldVelocities.Last();
//	}
//
//	//get the angle between the velocities of the 2 balls
//	const float Angle = FMath::Acos(FVector::DotProduct(OldVelocity.GetSafeNormal(), Hit.ImpactNormal));
//
//	////get the angle between the velocities of the 2 balls
//	//const float Angle2 = FMath::Acos(FVector::DotProduct(GetBallVelocity().GetSafeNormal(), OtherBallActor->SphereComponent->GetComponentLocation() - Hit.Location));
//
//	//the direction that the other ball will head in
//	const FVector OtherBallDirection = (OtherBallActor->SphereComponent->GetComponentLocation() - Hit.Location).GetSafeNormal();
//
//	//get our new velocity
//	FVector OurNewVel = OldVelocity * FMath::Sin(Angle) * OldVelocity.GetSafeNormal() * BallBallCollisionMultiplier;
//
//	//get the other balls new velocity
//	FVector OtherBallNewVel = OldVelocity * FMath::Cos(Angle) * BallBallCollisionMultiplier * OtherBallDirection;
//
//	//draw a debug arrow in the 2 directions
//	DrawDebugDirectionalArrow(GetWorld(), Hit.Location, Hit.Location + OurNewVel, 100, FColor::Green, false, 5);
//	DrawDebugDirectionalArrow(GetWorld(), Hit.Location, Hit.Location + OtherBallNewVel, 100, FColor::Red, false, 5);
//
//	//print our variables
//	UE_LOG(LogTemp, Log, TEXT("ABallActor::SolveStationaryBallMovingBallCollision: OurNewVel: %s, OtherBallNewVel: %s, OldVelocity: %s"), *OurNewVel.ToString(), *OtherBallNewVel.ToString(), *OldVelocity.ToString());
//
//	//set the new velocities
//	SetBallVelocity(OurNewVel);
//	OtherBallActor->SetBallVelocity(OtherBallNewVel);
//
//	//return true
//	return true;
//}
//
//bool ABallActor::SolveMovingBallMovingBallCollision(const FHitResult& Hit, const TObjectPtr<ABallActor>& OtherBallActor)
//{
//	//storage for the old velocity
//	FVector OldVelocity = GetBallVelocity();
//
//	//check if we have any old velocities
//	if (OldVelocities.Num() > 0)
//	{
//		//set the old velocity to the last element in the array
//		OldVelocity = OldVelocities.Last();
//	}
//
//	//get the other balls velocity
//	FVector OtherBallOldVel = OtherBallActor->GetBallVelocity();
//
//	//check if the other ball has any old velocities
//	if (OtherBallActor->OldVelocities.Num() > 0)
//	{
//		//set the other balls old velocity to the last element in the array
//		OtherBallOldVel = OtherBallActor->OldVelocities.Last();
//	}
//
//	//our calculated old velocity
//	FVector CalcOldVel = OldVelocity - OtherBallOldVel;
//
//	//get the angle between the velocities of the 2 balls
//	const float Angle = FMath::Acos(FVector::DotProduct(OldVelocity.GetSafeNormal(), Hit.ImpactNormal));
//
//	//our new velocity
//	FVector OurNewVel = CalcOldVel* FMath::Sin(Angle) * CalcOldVel.GetSafeNormal();
//
//	//the other balls new velocity
//	FVector OtherBallNewVel = CalcOldVel * FMath::Cos(Angle) * OtherBallOldVel.GetSafeNormal();
//
//	//print our variables
//	UE_LOG(LogTemp, Log, TEXT("ABallActor::SolveMovingBallMovingBallCollision OurNewVel: %s, OtherBallNewVel: %s, OldVelocity: %s, OtherBallOldVelocity: %s, CalcOldVel: %s"), *OurNewVel.ToString(), *OtherBallNewVel.ToString(), *OldVelocity.ToString(), *OtherBallOldVel.ToString(), *CalcOldVel.ToString());
//
//	//set the new velocities
//	SetBallVelocity(OurNewVel);
//	OtherBallActor->SetBallVelocity(OtherBallNewVel);
//
//	//return true
//	return true;
//}

float ABallActor::GetFrictionBetweenBalls(const TObjectPtr<ABallActor>& OtherBallActor) const
{
	//our velocity projected onto the x axis
	FVector v1_c = GetBallVelocity() + FVector::CrossProduct(GetBallAngularVelocity(), SphereComponent->GetScaledSphereRadius() * FVector::XAxisVector) - FVector(GetBallVelocity().X, 0, 0);

	//the other balls velocity projected onto the x axis
	FVector v2_c = OtherBallActor->GetBallVelocity() + FVector::CrossProduct(OtherBallActor->GetBallAngularVelocity(), OtherBallActor->SphereComponent->GetScaledSphereRadius() * -FVector::XAxisVector) - FVector(OtherBallActor->GetBallVelocity().X, 0, 0);

	//the relative surface speed
    float relative_surface_speed = (v1_c - v2_c).Length();

	//calculate the friction between the 2 balls
    float ReturnFriction = BallBallFrictionA + BallBallFrictionB * FMath::Pow(-BallBallFrictionC * relative_surface_speed, 2);

	//return the friction
	return ReturnFriction;
}

void ABallActor::DrawVelChangeDebugArrows()
{
	//the colour to use for the old velocity arrow
	const FColor OldVelColour = UniqueDebugArrows ? FColor::Blue : FColor::Green;

	//the colour to use for the new velocity arrow
	const FColor NewVelColour = UniqueDebugArrows ? FColor::Yellow : FColor::Red;

	//the depth priority group to use
	const int32 DepthPriority = UniqueDebugArrows ? 0 : 0;

	//the thickness of the arrow
	const float Thickness = UniqueDebugArrows ? 1 : .5;

	//draw a debug arrow in the old velocity direction
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + OldVelocities.Last(), 100, OldVelColour, UniqueDebugArrows, 5, DepthPriority, Thickness);

	//draw a debug arrow in the new velocity direction
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + GetBallVelocity(), 100, NewVelColour, UniqueDebugArrows, 5, DepthPriority, Thickness);
}

FVector ABallActor::GetExitVelocityAfterWallCollision(const FHitResult& Hit)
{
	//check if we don't have a valid physics data blueprint
	if (!PhysicsDataBlueprint->IsValidLowLevelFast())
	{
		//return the reflection vector
		return UKismetMathLibrary::GetReflectionVector(GetBallVelocity(), Hit.ImpactNormal).GetSafeNormal();
	}

	//process the hit
	ProcessWallHit(Hit);

	//get the physics data blueprint
	const TObjectPtr<UPhysicsDataBlueprint> PhysicsDataBP = PhysicsDataBlueprint.GetDefaultObject();

	//get the exit direction
	const FVector ExitDirection = PhysicsDataBP->WallCollisionSetExitDirection(SuggestedVelocity, this, Hit);

	//get the exit speed
	const float ExitSpeed = PhysicsDataBP->WallCollisionSetExitSpeed(SuggestedVelocity, this, Hit);

	//set our new velocity
	return ExitDirection * ExitSpeed;
}

FVector ABallActor::GetExitVelocityAfterBallCollision(const FHitResult& Hit, ABallActor* OtherBallActor)
{
	//check if we don't have a valid physics data blueprint
	if (!PhysicsDataBlueprint->IsValidLowLevelFast())
	{
		//return the reflection vector
		return UKismetMathLibrary::GetReflectionVector(GetBallVelocity(), Hit.ImpactNormal).GetSafeNormal();
	}

	//get the physics data blueprint
	const TObjectPtr<UPhysicsDataBlueprint> PhysicsDataBP = PhysicsDataBlueprint.GetDefaultObject();

	//process the hit
	ProcessBallHit(OtherBallActor, Hit);

	//get the exit direction
	const FVector ExitDirection = PhysicsDataBP->BallCollisionSetExitDirection(SuggestedVelocity, this, OtherBallActor, Hit);

	//get the exit speed
	const float ExitSpeed = PhysicsDataBP->BallCollisionSetExitSpeed(SuggestedVelocity, this, OtherBallActor, ExitDirection, Hit);

	//return the exit velocity
	return ExitDirection * ExitSpeed;
}

FVector ABallActor::GetOtherBallExitVelocityAfterBallCollision(const FHitResult& Hit, ABallActor* OtherBallActor)
{
	//check if we don't have a valid physics data blueprint
	if (!PhysicsDataBlueprint->IsValidLowLevelFast())
	{
		//return the reflection vector
		return UKismetMathLibrary::GetReflectionVector(GetBallVelocity(), Hit.ImpactNormal).GetSafeNormal();
	}

	//get the physics data blueprint
	const TObjectPtr<UPhysicsDataBlueprint> PhysicsDataBP = PhysicsDataBlueprint.GetDefaultObject();

	//get the exit direction
	const FVector ExitDirection = PhysicsDataBP->OtherBallCollisionSetExitDirection(OtherBallSuggestedVelocity, this, OtherBallActor, Hit);

	//get the exit speed
	const float ExitSpeed = PhysicsDataBP->OtherBallCollisionSetExitSpeed(OtherBallSuggestedVelocity, this, OtherBallActor, ExitDirection, Hit);

	//return the exit velocity
	return ExitDirection * ExitSpeed;
}

bool ABallActor::ProcessBallHit(AActor* OtherActor, const FHitResult& Hit)
{
	//check if the other actor is not a ball actor
	if (!OtherActor->IsA(StaticClass()))
	{
		//return false
		return false;
	}

	//get the other ball actor
	const TObjectPtr<ABallActor> OtherBallActor = Cast<ABallActor>(OtherActor);

	//check if we're only processing collisions on the highest speed actor and the other balls velocity is greater than our velocity
	if (bOnlyProcessHighestSpeedCollision && GetBallVelocity().Length() < OtherBallActor->GetBallVelocity().Length())
	{
		//return early to prevent further execution
		return false;
	}

	//check if the other actor is a cue ball
	if (OtherActor-ActorHasTag(FName("CueBallTag")))
	{
		//get the cue ball
		const TObjectPtr<ABallActor> CueBall = Cast<ABallActor>(OtherActor);

		//loop through the ball upgrade data assets
		for (const auto BallUpgradeDataAsset : BallUpgradeDataAssets)
		{
			//call the OnCueBallHit function
			BallUpgradeDataAsset.Get()->GetDefaultObject<UBallUpgradeDataAsset>()->OnCueBallHit(this, CueBall, Hit);
		}
	}
	else
	{
		//loop through the ball upgrade data assets
		for (const auto BallUpgradeDataAsset : BallUpgradeDataAssets)
		{
			//call the OnNormalBallHit function
			BallUpgradeDataAsset.Get()->GetDefaultObject<UBallUpgradeDataAsset>()->OnNormalBallHit(this, OtherBallActor, Hit);
		}
	}

	//check if we're not using custom collision response
	if (!bUseCustomCollisionResponse)
	{
		//return early to prevent further execution
		return true;
	}

	//get the average restitution between the 2 balls
	float e_b = (this->BallRestitution + OtherBallActor->BallRestitution) / 2;

	//the delta between the 2 balls positions
	const FVector DeltaCenters = OtherBallActor->GetActorLocation() - GetActorLocation();

	//the tetha between the 2 balls
	const float Angle = FMath::Acos(FVector::DotProduct(DeltaCenters.GetSafeNormal(), FVector::XAxisVector));
	
	//the delta quaternion for translating to a reference frame where the vector between the 2 balls is the x axis
	const FQuat DeltaQuat = FQuat(FVector::ZAxisVector, Angle);

	//storage for our old velocities
	FVector OldVel = GetBallVelocity();
	FVector OtherBallOldVel = OtherBallActor->GetBallVelocity();
	FVector OldAngVel = GetBallAngularVelocity();
	FVector OtherBallOldAngVel = OtherBallActor->GetBallAngularVelocity();

	//the velocity of the 2 balls in the reference frame
	FVector OurVel = DeltaQuat.UnrotateVector(OldVel);
	FVector OtherBallVel = DeltaQuat.UnrotateVector(OtherBallOldVel);
	FVector OurAngVel = DeltaQuat.UnrotateVector(OldAngVel);
	FVector OtherBallAngVel = DeltaQuat.UnrotateVector(OtherBallOldAngVel);

	//the normal component of the velocities
	float OurNormComp = 0.5 * (1 - e_b) * FVector::DotProduct(OurVel, FVector::XAxisVector) + (1 + e_b) * FVector::DotProduct(OtherBallVel, FVector::XAxisVector);
	float OtherBallNormComp = 0.5 * (1 + e_b) * FVector::DotProduct(OurVel, FVector::XAxisVector) + (1 - e_b) * FVector::DotProduct(OtherBallVel, FVector::XAxisVector);

	//the normal component magnitude
	float NormCompMag = FMath::Abs(OurNormComp - OtherBallNormComp);

	//discarding the normal components for now
	OurVel.X = 0;
	OtherBallVel.X = 0;

	//storage for the old velocities
	FVector OurVel_f = OurVel;
	FVector OtherBallVel_f = OtherBallVel;
	FVector OurAngVel_f = OurAngVel;
	FVector OtherBallAngVel_f = OtherBallAngVel;

	//the contact point velocities
	FVector V1_C = OurVel + FVector::CrossProduct(OurAngVel, SphereComponent->GetScaledSphereRadius() * FVector::XAxisVector);
	FVector V2_C = OtherBallVel + FVector::CrossProduct(OtherBallAngVel, OtherBallActor->SphereComponent->GetScaledSphereRadius() * -FVector::XAxisVector);

	//the relative contact velocity
	FVector V12_C = V1_C - V2_C;

	//the new relative contact velocity
	FVector V12_C_Slip = V1_C - V2_C;

	////print our variables
	//UE_LOG(LogTemp, Log, TEXT("ABallActor::ProcessBallHit: OurVel: %s, OtherBallVel: %s, OurAngVel: %s, OtherBallAngVel: %s, OurNormComp: %f, OtherBallNormComp: %f, NormCompMag: %f, V1_C: %s, V2_C: %s, V12_C: %s"), *OurVel.ToString(), *OtherBallVel.ToString(), *OurAngVel.ToString(), *OtherBallAngVel.ToString(), OurNormComp, OtherBallNormComp, NormCompMag, *V1_C.ToString(), *V2_C.ToString(), *V12_C.ToString());

	//whether or not the balls have relative velocity against each other that is greater than the threshold
	bool HasRelativeVel = V12_C.Length() > 1e-9;

	//check if the balls have relative velocity
	if (HasRelativeVel)
	{
		//tangent components for the balls
		FVector D_V1_t = GetFrictionBetweenBalls(OtherBallActor) * NormCompMag * -V12_C.GetSafeNormal();
		FVector D_W1 = 2.5 / SphereComponent->GetScaledSphereRadius() * FVector::CrossProduct(FVector::XAxisVector, D_V1_t);

		//add the tangent components to the balls velocities
		OurVel_f += D_V1_t;
		OurAngVel_f += D_W1;
		OtherBallVel_f -= D_V1_t;
		OtherBallAngVel_f += D_W1;

		//calculate the new velocities at the contact points
		V1_C = OurVel_f + FVector::CrossProduct(OurAngVel_f, SphereComponent->GetScaledSphereRadius() * FVector::XAxisVector);
		V2_C = OtherBallVel_f + FVector::CrossProduct(OtherBallAngVel_f, OtherBallActor->SphereComponent->GetScaledSphereRadius() * -FVector::XAxisVector);

		//calculate the new relative contact velocity
		V12_C_Slip = V1_C - V2_C;
	}

	//check if we don't have relative velocity or the relative contct velocity has changed
	if (!HasRelativeVel || FVector::DotProduct(V12_C, V12_C_Slip) <= 0)
	{
		//the velocity tangent
		FVector TangentVel = -(1.0 / 7.0) * (
		    OurVel - OtherBallVel + SphereComponent->GetScaledSphereRadius() * FVector::CrossProduct(OurAngVel + OtherBallAngVel, FVector::UnitX())
		);

		//the angular velocity tangent
		FVector TangentAngVel = -(5.0 / 14.0) * (
            FVector::CrossProduct(FVector::UnitX() * OurVel - OtherBallVel, OurVel - OtherBallVel) / SphereComponent->GetScaledSphereRadius() + OurAngVel + OtherBallAngVel
        );

		//add the tangent components to the balls velocities
		OurVel_f = OurVel + TangentVel;
		OurAngVel_f = OurAngVel + TangentAngVel;
		OtherBallVel_f = OtherBallVel - TangentVel;
		OtherBallAngVel_f = OtherBallAngVel + TangentAngVel;
	}

	//reintroduce the final normal components
	OurVel_f.X = OurNormComp;
	OtherBallVel_f.X = OtherBallNormComp;

	//rotate the velocities back to the world frame
	OurVel_f = DeltaQuat.RotateVector(OurVel_f);
	OurAngVel_f = DeltaQuat.RotateVector(OurAngVel_f);
	OtherBallVel_f = DeltaQuat.RotateVector(OtherBallVel_f);
	OtherBallAngVel_f = DeltaQuat.RotateVector(OtherBallAngVel_f);

	//remove any z components from the velocities
	OurVel_f.Z = 0;
	OtherBallVel_f.Z = 0;

	//the ratio between the sum of the old velocities and the sum of the new velocities
	float SpeedRatio = (OurVel.Size() + OtherBallVel.Size()) * MaxRelativeSpeedGain / (OurVel_f.Size() + OtherBallVel_f.Size());

	//check if the sum of our new velocities are bigger than the sum of the old velocities
	if (MaxRelativeSpeedGain > 0 && (OurVel_f.Size() + OtherBallVel_f.Size()) * MaxRelativeSpeedGain > GetBallVelocity().Size() + OtherBallActor->GetBallVelocity().Size())
	{
		////the ratio between our speed and the other balls speed
		//float Ratio = OurVel_f.Size() / OtherBallVel_f.Size();

		//adjust the velocities
		OurVel_f *= SpeedRatio;
		OtherBallVel_f *= SpeedRatio;
	}

	////set the new velocities
	//this->SetBallVelocity(OurVel_f * BallBallCollisionMultiplier);
	//OtherBallActor->SetBallVelocity(OtherBallVel_f * BallBallCollisionMultiplier);

	//set the suggested velocities
	SuggestedVelocity = OurVel_f * BallBallCollisionMultiplier;
	OtherBallSuggestedVelocity = OtherBallVel_f * BallBallCollisionMultiplier;

	//set both balls to be rolling
	SetBoolPhysicsState(Ebps_Rolling);
	OtherBallActor->SetBoolPhysicsState(Ebps_Rolling);

	//check if we're in debug mode and we're displaying the debug arrows
	if (bDebugMode && BallColDebugArrows)
	{
		//draw the debug arrows
		DrawVelChangeDebugArrows();
	}

	//return true
	return true;
}

void ABallActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	////process the hit as if it was a ball hit
	//if(ProcessBallHit(OtherActor, SweepResult))
	//{
	//	//if it was a ball return early
	//	return;
	//}

	////process the hit as if it was a wall hit
	//ProcessWallHit(SweepResult);

	//check if we don't have a valid physics data blueprint
	if (!PhysicsDataBlueprint->IsValidLowLevelFast())
	{
		//reset our velocities
		ErrorResetVelocities(GetActorNameOrLabel() + " Has No Valid PhysicsDataBlueprint");

		//return early to prevent further execution
		return;
	}

	//get the physics data blueprint
	const TObjectPtr<UPhysicsDataBlueprint> PhysicsDataBP = PhysicsDataBlueprint.GetDefaultObject();

	//check if we're overlapping multiple actors
	if (OverlappingActors.Num() > 1)
	{
		//reset our velocities
		ErrorResetVelocities();

		//return early to prevent further execution
		return;
	}
	
	//check if we're overlapping a ball actor
	if (OtherActor->IsA(StaticClass()))
	{
		//cast the other actor to a ball actor
		const TObjectPtr<ABallActor> OtherBallActor = Cast<ABallActor>(OtherActor);

		//get our exit velocity after the ball collision
		const FVector ExitVelocity = GetExitVelocityAfterBallCollision(SweepResult, OtherBallActor.Get());

		//get the exit velocity for the other ball
		const FVector OtherBallExitVelocity = GetOtherBallExitVelocityAfterBallCollision(SweepResult, OtherBallActor.Get());

		//set our new velocity
		SetBallVelocity(ExitVelocity);

		//set the other balls new velocity
		OtherBallActor->SetBallVelocity(OtherBallExitVelocity);

		//return early to prevent further execution
		return;
	}

	//check if we're overlapping a goal actor
	if (OtherActor->IsA(AGoalActor::StaticClass()))
	{
		//return early to prevent further execution
		return;
	}

	//assume we're overlapping a wall actor

	//get the exit velocity after the wall collision
	const FVector ExitVelocity = GetExitVelocityAfterWallCollision(SweepResult);

	//set our new velocity
	SetBallVelocity(ExitVelocity);
}

void ABallActor::ProcessWallHit(const FHitResult& Hit)
{
	//check if the other actor is a goal actor
	if (Hit.GetActor()->IsA(AGoalActor::StaticClass()))
	{
		//return early to prevent further execution
		return;
	}

	//iterate over the ball upgrade data assets
	for (const auto BallUpgradeDataAsset : BallUpgradeDataAssets)
	{
		//call the OnWallHit function
		BallUpgradeDataAsset.Get()->GetDefaultObject<UBallUpgradeDataAsset>()->OnWallHit(this, Hit.Component.Get(), Hit);
	}

	//check if we're not using custom collision response
	if (!bUseCustomCollisionResponse)
	{
		//return early to prevent further execution
		return;
	}

	//the normal of the hit reversed
	const FVector ReversedNormal = Hit.ImpactNormal;

	//the angle between the normal and the x axis
	const float Angle = FMath::Acos(FVector::DotProduct(ReversedNormal, FVector::XAxisVector));

	//the angle quaternion
	const FQuat AngleQuat = FQuat(FVector::ZAxisVector, Angle).GetNormalized();

	////a 3x3 matrix with the actor location, velocity, and angular velocity transposed
	//TArray rvw_R ={
	//	FVector(/*GetActorLocation().X*/ 0, GetBallVelocity().X, GetBallAngularVelocity().X),
	//	FVector(0, GetBallVelocity().Y, GetBallAngularVelocity().Y),
	//	FVector(0, GetBallVelocity().Z, GetBallAngularVelocity().Z),
	//};

	TArray rvw_R ={
		AngleQuat.UnrotateVector(GetActorLocation()),
		AngleQuat.UnrotateVector(GetBallVelocity()),
		AngleQuat.UnrotateVector(GetBallAngularVelocity()),
	};

	////rotate the matrix
	//rvw_R = {
	//	AngleQuat.UnrotateVector(rvw_R[0]),
	//	AngleQuat.UnrotateVector(rvw_R[1]),
	//	AngleQuat.UnrotateVector(rvw_R[2]),
	//};

	////transpose the matrix again
	//rvw_R = {
	//	FVector(rvw_R[0].X, rvw_R[1].X, rvw_R[2].X),
	//	FVector(rvw_R[0].Y, rvw_R[1].Y, rvw_R[2].Y),
	//	FVector(rvw_R[0].Z, rvw_R[1].Z, rvw_R[2].Z),
	//};

	//the second angle
	const float SecondAngle = FMath::Acos(FVector::DotProduct(rvw_R[1].GetSafeNormal(), FVector::XAxisVector));

	//the rotated velocity
	const float phi = remainder(SecondAngle, 2 * UE_PI);

	//get mu and e
	const float mu = WallFriction;
	const float e = WallRestitution;

	//the angle between the hit normal and the z axis
	const float theta_a = FMath::Asin((Hit.Location.Z - GetActorLocation().Z) / (SphereComponent->GetScaledSphereRadius() - 1));

	//the angular component of the collision
	const float sx = rvw_R[1].X * FMath::Sin(theta_a) - rvw_R[1].Z * FMath::Cos(theta_a) + SphereComponent->GetScaledSphereRadius() * rvw_R[2].Y;
	const float sy = 
		-rvw_R[1].Y
	    - SphereComponent->GetScaledSphereRadius() * rvw_R[2].Z * FMath::Cos(theta_a)
	    + SphereComponent->GetScaledSphereRadius() * rvw_R[2].X * FMath::Sin(theta_a);

	//the energy component of the collision
	const float c = rvw_R[1].X * FMath::Cos(theta_a);

	//the angular momentum component of the collision
	float II = .4 * BallMass * FMath::Pow(SphereComponent->GetScaledSphereRadius(), 2);
	const float A = 7 / 2 / BallMass;
	const float B = 1 / BallMass;

    //the first and second check variables
	const float PzE = (1 + e) * c / B;
	const float PzS = FMath::Sqrt(FMath::Pow(sx, 2) + FMath::Pow(sy, 2)) / A;

	float PX, PY, PZ;

    if (PzS <= PzE)
    {
		//Sliding and sticking case
		PX = -sx / A * FMath::Sin(theta_a) - (1 + e) * c / B * FMath::Cos(theta_a);
        PY = sy / A;
        PZ = sx / A * FMath::Cos(theta_a) - (1 + e) * c / B * FMath::Sin(theta_a);
    }
    else
    {
	    //Forward sliding case
        PX = -mu * (1 + e) * c / B * FMath::Cos(phi) * FMath::Sin(theta_a) - (
            1 + e
        ) * c / B * FMath::Cos(theta_a);
        PY = mu * (1 + e) * c / B * FMath::Sin(phi);
        PZ = mu * (1 + e) * c / B * FMath::Cos(phi) * FMath::Cos(theta_a) - (
            1 + e
        ) * c / B * FMath::Sin(theta_a);
    }

    //Update velocity
    rvw_R[1].X += PX / BallMass;
    rvw_R[1].Y += PY / BallMass;
    //rvw_R[1].Z += PZ / BallMass;

    //Update angular velocity
    rvw_R[2].X += -SphereComponent->GetScaledSphereRadius() / II * PY * FMath::Sin(theta_a);
    rvw_R[2].Y += SphereComponent->GetScaledSphereRadius() / II * (PX * FMath::Sin(theta_a) - PZ * FMath::Cos(theta_a));
    rvw_R[2].Z += SphereComponent->GetScaledSphereRadius() / II * PY * FMath::Cos(theta_a);

	////transpose the matrix again
	//TArray rvw_R2 = {
	//	FVector(rvw_R[0].X, rvw_R[1].X, rvw_R[2].X),
	//	FVector(rvw_R[0].Y, rvw_R[1].Y, rvw_R[2].Y),
	//	FVector(rvw_R[0].Z, rvw_R[1].Z, rvw_R[2].Z),
	//};

	TArray rvw_R2 = {
		rvw_R[0],
		rvw_R[1],
		rvw_R[2],
	};

	//unrotate the matrix
	rvw_R2 = {
		AngleQuat.RotateVector(rvw_R2[0]),
		AngleQuat.RotateVector(rvw_R2[1]),
		AngleQuat.RotateVector(rvw_R2[2]),
	};

	////transpose the matrix again
	//rvw_R2 = {
	//	FVector(rvw_R2[0].X, rvw_R2[1].X, rvw_R2[2].X),
	//	FVector(rvw_R2[0].Y, rvw_R2[1].Y, rvw_R2[2].Y),
	//	FVector(rvw_R2[0].Z, rvw_R2[1].Z, rvw_R2[2].Z),
	//};

	//set the new position
	SetActorLocation(/*GetActorLocation() +*/ rvw_R2[0]);

	////set the new velocity
	//SetBallVelocity(FVector(rvw_R2[1].X, rvw_R2[1].Y, 0));

	////set the new angular velocity
	//SetBallAngularVelocity(rvw_R2[2]);

	//set the suggested new velocity
	SuggestedVelocity = FVector(rvw_R2[1].X, rvw_R2[1].Y, 0);

	//set the suggested new angular velocity
	AngularVelocity = rvw_R2[2];

	//check if we're in debug mode and we're displaying the debug arrows
	if (bDebugMode && WallColDebugArrows)
	{
		//draw the debug arrows
		DrawVelChangeDebugArrows();
	}
}

void ABallActor::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	////check if the other actor is a ball actor
	//if (ProcessBallHit(OtherActor, Hit))
	//{
	//	//return early to prevent further execution
	//	return;
	//};

	////we're assuming the other actor is a wall
	//ProcessWallHit(Hit);
}

void ABallActor::BallBeginDetectionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//add the other actor to the list of overlapping actors
	OverlappingActors.AddUnique(OtherActor);
}

void ABallActor::BallEndDetectionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//remove the other actor from the list of overlapping actors
	OverlappingActors.Remove(OtherActor);
}

FVector ABallActor::GetBallVelocity() const
{
	////check if we're not using custom collision response
	//if (!bUseCustomCollisionResponse)
	//{
	//	//return the physics linear velocity
	//	return SphereComponent->GetPhysicsLinearVelocity();
	//}

	//return the current velocity variable
	return CurrentVelocity;
}

FVector ABallActor::GetBallAngularVelocity() const
{
	////check if we're not using custom collision response
	//if (!bUseCustomCollisionResponse)
	//{
	//	//return the physics angular velocity
	//	return SphereComponent->GetPhysicsAngularVelocityInRadians();
	//}

	//return the angular velocity variable
	return AngularVelocity;
}

//float ABallActor::GetFrictionCoefficient() const
//{
//	//check the physics state
//	switch (PhysicsState)
//	{
//	case Ebps_Spinning:
//	{
//		//return the spinning friction coefficient
//		return TableSpinningFrictionCoefficient;
//	}
//	case Ebps_Rolling:
//	{
//		//return the rolling friction coefficient
//		return TableRollingFrictionCoefficient;
//	}
//	case Ebps_Sliding:
//	{
//		//return the sliding friction coefficient
//		return TableSlidingFrictionCoefficient;
//	}
//	default:
//	{
//		//return 0
//		return 0;
//	}
//	}
//}
//
//bool ABallActor::IsBallStationary() const
//{
//	//check if we're not using custom collision response
//	if (!bUseCustomCollisionResponse)
//	{
//		//return if the physics linear velocity is less than the stationary speed
//		return SphereComponent->GetPhysicsLinearVelocity().Size() <= StationarySpeed;
//	}
//
//	//return if whether or not the bool physics state is stationary or spinning
//	return PhysicsState == Ebps_Stationary || PhysicsState == Ebps_Spinning;
//}

void ABallActor::SetBallVelocity(const FVector& NewVelocity)
{
	////check if we're not using custom collision response
	//if (!bUseCustomCollisionResponse)
	//{
	//	//set all physics linear velocity
	//	SphereComponent->SetAllPhysicsLinearVelocity(NewVelocity);
	//}

	//set the current velocity variable
	CurrentVelocity = NewVelocity;
}

void ABallActor::SetBallAngularVelocity(const FVector& NewAngularVelocity)
{
	////check if we're not using custom collision response
	//if (!bUseCustomCollisionResponse)
	//{
	//	//set the physics angular velocity
	//	SphereComponent->SetAllPhysicsAngularVelocityInRadians(NewAngularVelocity);
	//}

	//set the angular velocity variable
	AngularVelocity = NewAngularVelocity;
}

void ABallActor::ErrorResetVelocities(const FString ErrorMessage, const bool bPrintCallStack)
{
	//check if we have an error message
	if (!ErrorMessage.IsEmpty())
	{
		//check if we're printing the call stack
		if (bPrintCallStack)
		{
			//dump the call stack to the log as an error
			FDebug::DumpStackTraceToLog(ELogVerbosity::Error);
		}

		//print the error message to the log
		UE_LOG(LogTemp, Error, TEXT("ABallActor::ErrorResetVelocities: %s"), *ErrorMessage);

		//print the error message to the screen
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, ErrorMessage);
	}

	//set our velocity to zero
	SetBallVelocity(FVector::ZeroVector);

	//set our angular velocity to zero
	SetBallAngularVelocity(FVector::ZeroVector);
}

//FString ABallActor::GetPhysicsStateAsString(EBallPhysicsState InPhysicsState) const
//{
//	//check the physics state
//	switch (InPhysicsState)
//	{
//		case Ebps_Spinning:
//		{
//			//return the spinning physics state as a string
//			return "Spinning";
//		}
//		case Ebps_Rolling:
//		{
//			//return the rolling physics state as a string
//			return "Rolling";
//		}
//		case Ebps_Sliding:
//		{
//			//return the sliding physics state as a string
//			return "Sliding";
//		}
//		case Ebps_Stationary:
//		{
//			//return the stationary physics state as a string
//			return "Stationary";
//		}
//		default:
//		{
//			//return a message saying the physics state is default
//			return "ABallActor::GetPhysicsStateAsString default";
//		}
//	}
//}

