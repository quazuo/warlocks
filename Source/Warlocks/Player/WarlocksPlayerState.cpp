#include "WarlocksPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AWarlocksPlayerState::AWarlocksPlayerState()
{
	bReplicates = true;
}

void AWarlocksPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWarlocksPlayerState, bIsDead);
	DOREPLIFETIME(AWarlocksPlayerState, bIsStunned);
	DOREPLIFETIME(AWarlocksPlayerState, bIsCastingSpell);
	DOREPLIFETIME(AWarlocksPlayerState, bIsChannelingSpell);
	DOREPLIFETIME(AWarlocksPlayerState, bIsVictorious);
}

void AWarlocksPlayerState::Reset()
{
	Super::Reset();
	
	const auto CDO = Cast<AWarlocksPlayerState>(StaticClass()->GetDefaultObject());
	if (!CDO) return;

	bIsDead = CDO->bIsDead;
	bIsStunned = CDO->bIsStunned;
	bIsCastingSpell = CDO->bIsCastingSpell;
	bIsChannelingSpell = CDO->bIsChannelingSpell;
	bIsVictorious = CDO->bIsVictorious;
}
