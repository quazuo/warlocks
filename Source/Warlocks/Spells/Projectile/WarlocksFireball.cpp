#include "WarlocksFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

AWarlocksFireball::AWarlocksFireball()
{
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(1, 0.5, 0));
	PointLight->SetupAttachment(RootComponent);

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);
}

TSubclassOf<UObject> AWarlocksFireball::GetBPClassPtr()
{
	const auto ObjPath =
		TEXT("/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Spells/BP_WarlocksFireball.BP_WarlocksFireball'");
	return FWarlocksUtils::GetBPClassPtr(ObjPath);
}

void AWarlocksFireball::BeginPlay()
{
	Super::BeginPlay();
}

void AWarlocksFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarlocksFireball::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	Super::OnHit(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (OtherActor == this || OtherActor == GetOwner() || Cast<AWarlocksSpell>(OtherActor))
		return;

	const auto OwnerWarlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!OwnerWarlock) return;
	
	if (const auto Enemy = Cast<AWarlocksCharacter>(OtherActor))
	{
		if (const auto OwnerController = OwnerWarlock->GetController())
		{
			UGameplayStatics::ApplyDamage(Enemy, Power, OwnerController, this, nullptr);
		}

		Enemy->Launch(-1 * SweepResult.Normal, Knockback);
		
		if (const auto EnemyState = Cast<AWarlocksPlayerState>(Enemy->GetPlayerState()))
		{
			EnemyState->bIsChannelingSpell = false;
		}

		UE_LOG(LogWarlocks, Error, TEXT("Fireball hit: %f damage"), Power);
	}

	SpawnOnHitParticle();
	Destroy();
}

void AWarlocksFireball::SpawnOnHitParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitParticle, GetActorLocation(),
		FRotator::ZeroRotator, FVector(.3));
}
