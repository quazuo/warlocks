#include "WarlocksSafeZone.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Warlocks/Game/WarlocksGameState.h"

AWarlocksSafeZone::AWarlocksSafeZone()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = Mesh;
	Mesh->SetWorldScale3D({RoundBeginSafeZoneScale, RoundBeginSafeZoneScale, .25});
	Mesh->SetIsReplicated(true);

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsuleComponent"));
	TriggerCapsule->SetupAttachment(RootComponent);
	TriggerCapsule->SetIsReplicated(true);
	
	bReplicates = true;
}

void AWarlocksSafeZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AWarlocksSafeZone, CurrentSafeZoneScale, COND_None, REPNOTIFY_Always);
}

float AWarlocksSafeZone::GetCurrentSafeZoneRadius() const
{
	const auto Width = Mesh->GetStaticMesh()->GetBoundingBox().GetSize().X;
	return Width * CurrentSafeZoneScale / 2;
}

void AWarlocksSafeZone::ResetSafeZone()
{
	UpdateMeshScale(RoundBeginSafeZoneScale);
	UpdateCapsuleSize();

	// setup shrinking of the safe zone on an interval
	FTimerDelegate ShrinkDelegate;
	ShrinkDelegate.BindUFunction(this, FName("ShrinkSafeZone"));
	GetWorldTimerManager().ClearTimer(ShrinkTimer);
	GetWorldTimerManager().SetTimer(ShrinkTimer, ShrinkDelegate, SafeZoneShrinkInterval, true);
}

void AWarlocksSafeZone::StopSafeZoneShrinking()
{
	GetWorldTimerManager().ClearTimer(ShrinkTimer);
}

void AWarlocksSafeZone::ShrinkSafeZone()
{
	if (CurrentSafeZoneScale == MinSafeZoneScale)
		return;

	const float NewMeshScale = UKismetMathLibrary::Max(
		MinSafeZoneScale,
		CurrentSafeZoneScale - SafeZoneShrinkDiff
	);
	UpdateMeshScale(NewMeshScale);
	UpdateCapsuleSize();
	
	AnnounceSafeZoneShrink();
}

void AWarlocksSafeZone::UpdateMeshScale(const float Scale)
{
	FVector CurrScale = Mesh->GetRelativeScale3D();
	CurrScale.X = Scale;
	CurrScale.Y = CurrScale.X;
	Mesh->SetRelativeScale3D(CurrScale);
	Mesh->SetWorldScale3D(CurrScale);
	CurrentSafeZoneScale = CurrScale.X;
}

void AWarlocksSafeZone::UpdateCapsuleSize() const
{
	const float Radius = GetCurrentSafeZoneRadius();
	TriggerCapsule->SetCapsuleSize(Radius, UKismetMathLibrary::Max(Radius, 500));
}

void AWarlocksSafeZone::RepNotify_CurrentSafeZoneScale() const
{
	AnnounceSafeZoneShrink();
}

void AWarlocksSafeZone::AnnounceSafeZoneShrink() const
{
	if (GetLocalRole() != ROLE_Authority) 
		return;
	
	const auto State = Cast<AWarlocksGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (State && State->GetAnnouncer())
	{
		State->GetAnnouncer()->Server_AnnounceSafeZoneShrink();
	}
}
