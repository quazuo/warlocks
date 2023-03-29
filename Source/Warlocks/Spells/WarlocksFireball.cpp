// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Warlocks/WarlocksCharacter.h"
#include "Warlocks/WarlocksPlayerController.h"

AWarlocksFireball::AWarlocksFireball()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionSphere->InitSphereRadius(15);
	CollisionSphere->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWarlocksFireball::OnHit);
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
	ProjectileMesh->SetRelativeScale3D(FVector(0.09, 0.09, 0.09));
	ProjectileMesh->SetupAttachment(RootComponent);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(1, 0.5, 0));
	PointLight->SetupAttachment(RootComponent);

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);

	InitialLifeSpan = 2;
}

void AWarlocksFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarlocksFireball::OnSpawn(const FVector& Direction)
{
	ProjectileMovement->Velocity = Direction * ProjectileSpeed;
	ProjectileMovement->InitialSpeed = ProjectileSpeed;
	ProjectileMovement->MaxSpeed = ProjectileSpeed;

	CollisionSphere->SetSphereRadius(ProjectileHitboxRadius);
}

void AWarlocksFireball::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner()) return;

	AWarlocksCharacter* Enemy = Cast<AWarlocksCharacter>(OtherActor);
	if (Enemy)
	{
		Enemy->Launch(-1 * SweepResult.Normal);

		const auto Controller = Cast<AWarlocksPlayerController>(Enemy->GetController());
		if (Controller)
		{
			Controller->StopChannelingSpell();
		}
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitParticle, GetActorLocation(),
		FRotator::ZeroRotator, FVector(.3));
	Destroy();
}
