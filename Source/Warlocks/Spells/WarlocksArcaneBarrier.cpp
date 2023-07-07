


#include "WarlocksArcaneBarrier.h"

#include "WarlocksFireball.h"

AWarlocksArcaneBarrier::AWarlocksArcaneBarrier()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = Mesh;
}

void AWarlocksArcaneBarrier::BeginPlay()
{
	auto Location = GetActorLocation();
	Location.Z = 0;
	SetActorLocation(Location);
	
	Super::BeginPlay();
}

void AWarlocksArcaneBarrier::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// todo maybe a particle
}
