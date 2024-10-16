// Fill out your copyright notice in the Description page of Project Settings.
#include "Core/PlayerPawn.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "Balls/BallUpgradeDataAsset.h"
#include "Balls/CueBall.h"
#include "Bool/GoalActor.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


APlayerPawn::APlayerPawn()
{
	//enable ticking
	PrimaryActorTick.bCanEverTick = true;

	//create the camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	//setup attachment(s)
	CameraComponent->SetupAttachment(RootComponent);

	//set the rotation of the camera component
	CameraComponent->SetRelativeRotation(FRotator(0,-90,0));
}

void APlayerPawn::BeginPlay()
{
	//call the parent implementation
	Super::BeginPlay();

	//get the player controller
	PlayerController = CastChecked<APlayerController>(GetController());

	//get the cue ball
	CurrentCueBall = Cast<ACueBall>(UGameplayStatics::GetActorOfClass(GetWorld(), ACueBall::StaticClass()));

	//set the player controller input mode
	PlayerController->SetInputMode(FInputModeGameAndUI());

	//set the player controller show mouse cursor
	PlayerController->bShowMouseCursor = true;

	//set the turns this round to the default turns per round
	TurnsThisRound = TurnsPerRound;
}

void APlayerPawn::Tick(float DeltaTime)
{
	//call the parent implementation
	Super::Tick(DeltaTime);

	//check if the shooting trajectory is locked
	if (!bLockedShotTrajectory)
	{
		//set the aim location to the current mouse world position
		AimLocation = GetMouseWorldPosition();
	}
	else
	{
		//set the current shot speed
		CurrentShotSpeed = FMath::Clamp(FVector::Dist(CurrentCueBall->GetActorLocation(), GetMouseWorldPosition()) * ShotSpeedMultiplier, MinimumShootingSpeed, MaxShootingSpeed);
	}

	//check if the turn is in progress and we can shoot (all balls are stopped)
	if (bTurnInProgress && CanShoot())
	{
		//set turn in progress to false
		bTurnInProgress = false;

		//set has ended turn to false
		bHasEndedTurn = false;
	}
	else if (CanShoot() && !bHasEndedTurn)
	{
		//set has ended turn to true 
		bHasEndedTurn = true;

		//call the OnTurnEnd function
		OnTurnEnd();
	}
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//call the parent implementation
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//check if we have a valid input data asset and enhanced input component
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent); EnhancedInputComponent->IsValidLowLevel())
	{
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Triggered, this, &APlayerPawn::ShootCueBall);
		EnhancedInputComponent->BindAction(IA_ResetAim, ETriggerEvent::Triggered, this, &APlayerPawn::ResetAim);
	}

	//check if we have a valid input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalViewingPlayerController()->GetLocalPlayer()))
	{
		//add the input mapping context
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void APlayerPawn::SetCueBallHitLocation(FVector2D HitLocation)
{
	//set the cue ball hit location
	CueBallHitLocation = HitLocation;

	//call convert location to cue ball
	ConvertLocationToCueBall(CueBallHitLocation);
}

void APlayerPawn::ShootCueBallAtPosition(FVector NewVelocity, FName BoneName)
{
	//convert the location to a point on the cue ball
	FVector LocationToHit = ConvertLocationToCueBall(CueBallHitLocation);

	//add impulse to the cue ball at the location
	CurrentCueBall->SphereComponent->AddImpulseAtLocation(NewVelocity, LocationToHit, BoneName);
}

FVector APlayerPawn::ConvertLocationToCueBall(FVector2D InLocation)
{
	//direction from the aim location to the cue ball
	FVector AimDirection = (CurrentCueBall->GetActorLocation() - AimLocation).GetSafeNormal();

	//storage for the hit result
	FHitResult HitResult;

	//do a line trace to get the hit result
	GetWorld()->LineTraceSingleByChannel(HitResult, AimLocation, AimLocation + AimDirection * 10000, ECC_Visibility);

	//check if the hit result is not valid
	if (!HitResult.IsValidBlockingHit())
	{
		//print a debug message
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Hit Result Found"));

		//return early to prevent further execution
		return FVector::ZeroVector;
	}

	
	//get the x axis direction
	FVector XAxisDirection = FVector::CrossProduct(AimDirection, FVector::UpVector).GetSafeNormal() * -1;

	//storage for the x axis location
	FVector XAxisLocation = CurrentCueBall->SphereComponent->GetScaledSphereRadius() * XAxisDirection * InLocation.X;

	//get the y axis direction
	FVector YAxisDirection = FVector::CrossProduct(AimDirection, FVector::RightVector).GetSafeNormal();

	//storage for the y axis location
	FVector YAxisLocation = CurrentCueBall->SphereComponent->GetScaledSphereRadius() * YAxisDirection * InLocation.Y;

	////add in the x axis value
	//ReturnValue += CurrentCueBall->SphereComponent->GetScaledSphereRadius() * cos(Latitude) * cos(Longitude) * FVector::ForwardVector;

	//set the return value
	FVector ReturnValue = CurrentCueBall->GetActorLocation() + XAxisLocation + YAxisLocation + HitResult.ImpactPoint - CurrentCueBall->GetActorLocation();

	//return the return result
	return ReturnValue;
	
}

bool APlayerPawn::CanShoot() const
{
	//check if we have a valid cue ball
	if (!CurrentCueBall)
	{
		//return false
		return false;
	}

	//get all ball actors in the world
	TArray<AActor*> BallActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABallActor::StaticClass(), BallActors);

	//loop through the ball actors
	for (AActor* BallActor : BallActors)
	{
		//check if the physics linear velocity is greater than the ball stop speed
		if (Cast<ABallActor>(BallActor)->SphereComponent->GetComponentVelocity().Size() > BallStopSpeed)
		{
			//return false
			return false;
		}
	}

	//return true
	return true;
}

void APlayerPawn::ShootCueBall(const FInputActionValue& Value)
{
	//check if there is a turn in progress or if we're not allowed to play
	if (bTurnInProgress || !bCanPlay)
	{
		//return early to prevent further execution
		return;
	}

	//check if the shooting trajectory is locked
	if (!bLockedShotTrajectory)
	{
		//toggle locking the shooting trajectory
		ToggleLockedShootingTrajectory();

		//return early to prevent further execution
		return;
	}

	//check if we have a valid cue ball
	if (CurrentCueBall)
	{
		//get the direction to shoot the cue ball
		const FVector Direction = (CurrentCueBall->GetActorLocation() - AimLocation).GetSafeNormal();

		//shoot the cue ball at the position
		ShootCueBallAtPosition(Direction * CurrentShotSpeed, NAME_None);

		////shoot the cue ball
		//CurrentCueBall->SphereComponent->AddImpulse(Direction * CurrentShotSpeed, NAME_None, true);

		//set turn in progress to true
		bTurnInProgress = true;

		//toggle locking the shooting trajectory
		ToggleLockedShootingTrajectory();
	}
}

void APlayerPawn::ResetAim(const FInputActionValue& Value)
{
	//set the locket shot trajectory
	bLockedShotTrajectory = false;
}

void APlayerPawn::ToggleLockedShootingTrajectory()
{
	//toggle the locked shot trajectory
	bLockedShotTrajectory = !bLockedShotTrajectory;
}

FVector APlayerPawn::GetMouseWorldPosition()
{
	//get the mouse position
	float MouseX;
	float MouseY;
	PlayerController->GetMousePosition(MouseX, MouseY);

	//convert the mouse position to a world location
	FVector WorldLocation;
	FVector WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

	//convert the location to actual world location
	FVector TrueWorldLocation = WorldLocation.Z / WorldDirection.Z * WorldDirection * -1 + WorldLocation;

	//check if we have a valid cue ball
	if (CurrentCueBall->IsValidLowLevelFast())
	{
		TrueWorldLocation.Z = CurrentCueBall->GetActorLocation().Z;
	}

	//return the world location
	return TrueWorldLocation;
}

void APlayerPawn::OnTurnEnd()
{
	//add on screen debug message
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Turn Ended. Current Turn: " + FString::FromInt(CurrentTurn)));

	//increment the current turn
	CurrentTurn++;

	//get all goal actors in the world
	TArray<AActor*> GoalActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoalActor::StaticClass(), GoalActors);

	//iterate through the goal actors
	for (AActor* GoalActor : GoalActors)
	{
		//get the goal actor
		AGoalActor* Goal = Cast<AGoalActor>(GoalActor);

		//get all ball actors in the goal
		TArray<AActor*> BallActors;
		Goal->BoxComponent->GetOverlappingActors(BallActors);

		//iterate through the ball actors
		for (AActor* BallActor : BallActors)
		{
			//get the ball actor
			ABallActor* Ball = Cast<ABallActor>(BallActor);

			//iterate through the ball upgrade data assets
			for (UBallUpgradeDataAsset* BallUpgradeDataAsset : Ball->BallUpgradeDataAssets)
			{
				//call the OnGoalHit function
				BallUpgradeDataAsset->OnGoal(Ball, Goal);
			}

			//check if the object is not a cueball
			if (!Ball->IsA(ACueBall::StaticClass()))
			{
				//destroy the ball actor
				Ball->Destroy();
			}
			else
			{
				//get the cue ball
				ACueBall* CueBall = Cast<ACueBall>(Ball);

				//set the location of the cue ball back to the start position
				CueBall->SetActorLocation(CueBall->StartPosition);

				//set the velocity to zero
				CueBall->SphereComponent->SetAllPhysicsLinearVelocity(FVector::Zero());
			}
		}
	}

	//call the blueprint OnTurnEnd function
	OnTurnEndBP();

	//check if the current turn is greater than or equal to the turns this round
	if (CurrentTurn >= TurnsThisRound)
	{
		//call the OnRoundEnd function
		OnRoundEnd();
	}
}

void APlayerPawn::OnRoundEnd()
{
	//set the current turn to 0
	CurrentTurn = 0;

	//set can play to false
	bCanPlay = false;

	//call the blueprint OnRoundEnd function
	OnRoundEndBP();
}
