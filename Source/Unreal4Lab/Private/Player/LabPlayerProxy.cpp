// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabBlueprintLibrary.h"
#include "LabPlayerProxy.h"
#include "AIController.h"
#include "LabPlayerState.h"
#include "Unreal4LabPlayerController.h"
#include "LabPawn.h"



ALabPlayerProxy::ALabPlayerProxy(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// It seems that without a RootComponent, we can't place the Actual Character easily
	TSubobjectPtr<UCapsuleComponent> TouchCapsule = PCIP.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("dummy"));
	TouchCapsule->InitCapsuleSize(1.0f, 1.0f);
	TouchCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TouchCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = TouchCapsule;

	// Create a camera boom...
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopDownCamera"));
	TopDownCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUseControllerViewRotation = false; // Camera does not rotate relative to arm

	if (Role == ROLE_Authority)
	{
		static ConstructorHelpers::FObjectFinder<UClass> PlayerPawnBPClass(TEXT("/Game/Blueprints/MyCharacter.MyCharacter_C"));
		CharacterClass = PlayerPawnBPClass.Object;
	}
	bSetTeamNum = false;
}

void ALabPlayerProxy::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		// Get current location of the Player Proxy
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = true;

		// Spawn the actual player character at the same location as the Proxy
		Character = Cast<ALabPawn>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, SpawnParams));

		if (Character)
		{
			setTeamNum();
				
			// We use the PlayerAI to control the Player Character for Navigation
			PlayerAI = GetWorld()->SpawnActor<AAIController>(GetActorLocation(), GetActorRotation());
			PlayerAI->Possess(Character);
		}
		
	}

}

void ALabPlayerProxy::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	if (Character)
	{
		// Keep the Proxy in sync with the real character
		FTransform CharTransform = Character->GetTransform();
		FTransform MyTransform = GetTransform();

		FTransform Transform;
		Transform.LerpTranslationScale3D(CharTransform, MyTransform, ScalarRegister(0.5f));

		SetActorTransform(Transform);

	}

	//由于时机问题，之前没有给英雄设置上Team
	if (HasAuthority())
	{
		if (!bSetTeamNum && PlayerState)
		{
			setTeamNum();
		}
	}
}

void ALabPlayerProxy::MoveToLocation(const AUnreal4LabPlayerController* controller, const FVector& DestLocation)
{
	/** Looks easy - doesn't it.
	*  What this does is to engage the AI to pathfind.
	*  The AI will then "route" the character correctly.
	*  The Proxy (and with it the camera), on each tick, moves to the location of the real character
	*
	*  And thus, we get the illusion of moving with the Player Character
	*/
	PlayerAI->MoveToLocation(DestLocation);
}

void ALabPlayerProxy::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> &OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to Everyone
	DOREPLIFETIME(ALabPlayerProxy, Character);
}

void ALabPlayerProxy::setTeamNum()
{
	if (Character)
	{
		//Set the Team for the hero
		ALabPlayerState* player_state = Cast<ALabPlayerState>(PlayerState);
		if (player_state){
			bSetTeamNum = true;
			Character->SetTeamNum(player_state->GetTeamNum());
		}
	}
	
}
