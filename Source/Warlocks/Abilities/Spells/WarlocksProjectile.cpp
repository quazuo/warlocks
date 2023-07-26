#include "WarlocksProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AWarlocksProjectile::AWarlocksProjectile()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionSphere->InitSphereRadius(15);
	CollisionSphere->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	if (GetLocalRole() == ROLE_Authority)
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWarlocksProjectile::OnHit);
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

	InitialLifeSpan = 10.f;
}

void AWarlocksProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->Velocity = GetActorRotation().Vector() * ProjectileSpeed;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;

	CollisionSphere->SetSphereRadius(ProjectileHitboxRadius);
}

void AWarlocksProjectile::SpawnOnHitParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitParticle, GetActorLocation(),
	                                         FRotator::ZeroRotator, FVector(.3));
}
