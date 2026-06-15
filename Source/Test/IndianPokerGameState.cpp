#include "IndianPokerGameState.h"
#include "Net/UnrealNetwork.h"
#include "IndianPokerPlayerController.h"

void AIndianPokerGameState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIndianPokerGameState, CurrentTurn);
}

void AIndianPokerGameState::OnRep_Turn()
{
    if (UWorld* W = GetWorld())
    {
        if (AIndianPokerPlayerController* PC =
            Cast<AIndianPokerPlayerController>(W->GetFirstPlayerController()))
        {
            PC->RefreshUI();
        }
    }
}
