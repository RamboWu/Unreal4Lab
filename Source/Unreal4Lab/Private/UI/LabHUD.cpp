// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPlayerState.h"
#include "LabPlayerController.h"
#include "LabPawn.h"
#include "LabHUD.h"


ALabHUD::ALabHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> BarFillObj(TEXT("/Game/UI/HUD/BarFill"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlayerTeamHPObj(TEXT("/Game/UI/HUD/PlayerTeamHealthBar"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> EnemyTeamHPObj(TEXT("/Game/UI/HUD/EnemyTeamHealthBar"));

	BarFillTexture = BarFillObj.Object;
	PlayerTeamHPTexture = PlayerTeamHPObj.Object;
	EnemyTeamHPTexture = EnemyTeamHPObj.Object;

	UIScale = 1;
}

void ALabHUD::DrawHUD()
{
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		UIScale = ViewportSize.X / 2048.0f;
	}

	DrawActorsHealth();
}


void ALabHUD::DrawActorsHealth()
{
	for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
	{
		ALabPawn* TestChar = Cast<ALabPawn>(*Iterator);
		if (TestChar != NULL && TestChar->GetHealth() > 0)
		{
			//AStrategyAIController* AIController = Cast<AStrategyAIController>(TestChar->Controller);
			//if (AIController != NULL && AIController->IsLogicEnabled())
			//{
				DrawHealthBar(TestChar, TestChar->GetHealth() / (float)TestChar->GetMaxHealth(), 18 * UIScale);
			//}
		}
	}
	/*ALabGameState* const MyGameState = GetWorld()->GetGameState<AStrategyGameState>();
	if (MyGameState)
	{
		// 0 - unknown/neutral team, two teams in total
		for (int8 Team = 1; Team < EStrategyTeam::MAX; Team++)
		{
			for (int32 i = 0; i < MyGameState->GetPlayerData(Team)->BuildingsList.Num(); i++)
			{
				if (MyGameState->GetPlayerData(Team)->BuildingsList[i].IsValid())
				{
					AStrategyBuilding* const TestBuilding = Cast<AStrategyBuilding>(MyGameState->GetPlayerData(Team)->BuildingsList[i].Get());
					if (TestBuilding != NULL && TestBuilding->GetHealth() > 0 && !TestBuilding->IsBuildFinished())
					{
						DrawHealthBar(TestBuilding, TestBuilding->GetHealth() / (float)TestBuilding->GetMaxHealth(), 30 * UIScale);
					}
				}
			}
		}
	}*/

}


void ALabHUD::DrawHealthBar(AActor* ForActor, float HealthPercentage, int32 BarHeight, int32 OffsetY) const
{
	FBox BB = ForActor->GetComponentsBoundingBox();
	FVector Center = BB.GetCenter();
	FVector Extent = BB.GetExtent();
	FVector2D Center2D = FVector2D(Canvas->Project(FVector(Center.X, Center.Y, Center.Z + Extent.Z)));
	float ActorExtent = 40;
	if (Cast<APawn>(ForActor) != NULL)
	{
		ALabPawn* StrategyChar = Cast<ALabPawn>(ForActor);
		if ((StrategyChar != NULL) && (StrategyChar->CapsuleComponent != NULL))
		{
			ActorExtent = StrategyChar->CapsuleComponent->GetScaledCapsuleRadius();
		}
	}
	/*else if (Cast<AStrategyBuilding>(ForActor) != NULL)
	{
		Center2D = FVector2D(Canvas->Project(ForActor->GetActorLocation()));
		ActorExtent = 60;
	}*/

	FVector Pos1 = Canvas->Project(FVector(Center.X, Center.Y - ActorExtent * 2, Center.Z + Extent.Z));
	FVector Pos2 = Canvas->Project(FVector(Center.X, Center.Y + ActorExtent * 2, Center.Z + Extent.Z));
	float HealthBarLength = (Pos2 - Pos1).Size2D();

	ALabPlayerController* MyPC = GetPlayerController();
	ALabPlayerState* MyPS = GetPlayerState();

	ILabTeamInterface* ActorTeam = InterfaceCast<ILabTeamInterface>(ForActor);
	UTexture2D* HealthBarTexture = EnemyTeamHPTexture;

	if (ActorTeam != NULL && MyPS != NULL && ActorTeam->GetTeamNum() == MyPS->GetTeamNum())
	{
		HealthBarTexture = PlayerTeamHPTexture;
	}
	float X = Center2D.X - HealthBarLength / 2;
	float Y = Center2D.Y + OffsetY;
	FCanvasTileItem TileItem(FVector2D(X, Y), HealthBarTexture->Resource, FVector2D(HealthBarLength * HealthPercentage, BarHeight), FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	TileItem.UV1 = FVector2D(HealthPercentage, 1.0f);

	Canvas->DrawItem(TileItem);
	//Fill the rest of health with gray gradient texture
	X = Center2D.X - HealthBarLength / 2 + HealthBarLength * HealthPercentage;
	Y = Center2D.Y + OffsetY;
	TileItem.Position = FVector2D(X, Y);
	TileItem.Texture = BarFillTexture->Resource;
	TileItem.UV1 = FVector2D(1.0f, 1.0f);
	TileItem.Size = FVector2D(HealthBarLength * (1.0f - HealthPercentage), BarHeight);
	TileItem.SetColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));
	Canvas->DrawItem(TileItem);
}


ALabPlayerController* ALabHUD::GetPlayerController() const
{
	return Cast<ALabPlayerController>(PlayerOwner);
}

ALabPlayerState* ALabHUD::GetPlayerState() const
{
	if (GetPlayerController())
		return Cast<ALabPlayerState>(GetPlayerController()->PlayerState);
	else
		return NULL;
}