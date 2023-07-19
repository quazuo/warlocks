#include "WarlocksArcaneBarrier.h"

#include "Warlocks/FWarlocksUtils.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Warlocks/Spells/Projectile/WarlocksProjectileSpell.h"

AWarlocksArcaneBarrier::AWarlocksArcaneBarrier()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = Mesh;

	if (GetLocalRole() == ROLE_Authority)
	{
		Mesh->OnComponentBeginOverlap.AddDynamic(this, &AWarlocksArcaneBarrier::OnHit);
	}
}

TSubclassOf<UObject> AWarlocksArcaneBarrier::GetBPClassPtr()
{
	const auto ObjPath =
		TEXT(
			"/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Spells/BP_WarlocksArcaneBarrier.BP_WarlocksArcaneBarrier'");
	return FWarlocksUtils::GetBPClassPtr(ObjPath);
}

void AWarlocksArcaneBarrier::BeginPlay()
{
	auto Location = GetActorLocation();
	Location.Z = 0;
	SetActorLocation(Location);

	Super::BeginPlay();
}

void AWarlocksArcaneBarrier::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner() || !GetOwner())
		return;

	if (const auto Projectile = Cast<AWarlocksProjectileSpell>(OtherActor))
	{
		Projectile->SpawnOnHitParticle();
		OtherActor->Destroy();
	}
}
