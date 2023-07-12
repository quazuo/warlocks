#include "WarlocksProjectileSpell.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarlocksFireball.h"

AWarlocksProjectileSpell::AWarlocksProjectileSpell()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionSphere->InitSphereRadius(15);
	CollisionSphere->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	if (GetLocalRole() == ROLE_Authority)
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWarlocksProjectileSpell::OnHit);
	RootComponent = CollisionSphere;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
	ProjectileMovement->SetUpdatedComponent(CollisionSphere);
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMesh->SetRelativeScale3D(FVector(0.1));
	ProjectileMesh->SetupAttachment(RootComponent);

	InitialLifeSpan = 10;
}

void AWarlocksProjectileSpell::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Velocity = GetActorRotation().Vector() * ProjectileSpeed;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;

	CollisionSphere->SetSphereRadius(ProjectileHitboxRadius);
}

void AWarlocksProjectileSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarlocksProjectileSpell::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{

}
