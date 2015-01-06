// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Unreal4Lab.h"
#include "LabPlayerController.h"
#include "LabPlayerProxy.h"
#include "AI/Navigation/NavigationSystem.h"

ALabPlayerController::ALabPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ALabPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void ALabPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ALabPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ALabPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALabPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALabPlayerController::MoveToTouchLocation);

	InputComponent->BindAxis("MoveForward", this, &ALabPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALabPlayerController::MoveRight);
}

void ALabPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "MoveToTouchLocation" + Hit.ImpactPoint.ToString());

		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ALabPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	
	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ALabPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	ServerSetNewMoveDestination(DestLocation);
}

void ALabPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ALabPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ALabPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		GetPawn()->AddMovementInput(Direction, Value);
		//AddMovementInput(Direction, Value);
	}
}

void ALabPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		//AddMovementInput(Direction, Value);
		GetPawn()->AddMovementInput(Direction, Value);
	}
}

bool ALabPlayerController::ServerSetNewMoveDestination_Validate(const FVector DestLocation)
{
	return true;
}

/* Actual implementation of the ServerSetMoveDestination method */
void ALabPlayerController::ServerSetNewMoveDestination_Implementation(const FVector DestLocation)
{
	ALabPlayerProxy* Pawn = Cast<ALabPlayerProxy>(GetPawn());
	if (Pawn)
	{
		UNavigationSystem* const NaDemoys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, Pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NaDemoys && (Distance > 120.0f))
		{
			//NaDemoys-&gt;SimpleMoveToLocation(this, DestLocation);
			Pawn->MoveToLocation(this, DestLocation);
		}
	}
}

