#include "WarlocksFlash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

AWarlocksFlash::AWarlocksFlash()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

TSubclassOf<UObject> AWarlocksFlash::GetBPClassPtr()
{
	const auto ObjPath =
		TEXT("/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Spells/BP_WarlocksFlash.BP_WarlocksFlash'");
	return FWarlocksUtils::GetBPClassPtr(ObjPath);
}

void AWarlocksFlash::BeginPlay()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;
	
	const auto State = Cast<AWarlocksPlayerState>(Warlock->GetPlayerState());
	if (!State) return;

	// spawn initial particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));

	// move caster
	//State->bIsChannelingSpell = false;

	if (Warlock->GetController())
		Warlock->GetController()->StopMovement();
	Warlock->GetCharacterMovement()->Velocity = FVector::Zero();
	Warlock->SetActorLocation(Warlock->GetActorLocation() + GetActorRotation().Vector() * Range);

	// spawn next particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));
	
	Super::BeginPlay();
}
