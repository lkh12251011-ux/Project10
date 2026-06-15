#include "IndianPokerPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "IndianPokerPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"  

AIndianPokerPlayerState::AIndianPokerPlayerState()
{
}

void AIndianPokerPlayerState::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AIndianPokerPlayerState, HeadCard);
    DOREPLIFETIME(AIndianPokerPlayerState, HandCards);
    DOREPLIFETIME(AIndianPokerPlayerState, WinCount);
    DOREPLIFETIME(AIndianPokerPlayerState, bActionSelected);
    DOREPLIFETIME(AIndianPokerPlayerState, CurrentAction);
    DOREPLIFETIME(AIndianPokerPlayerState, bSaved);
    DOREPLIFETIME(AIndianPokerPlayerState, PlayerIndex);
}

void AIndianPokerPlayerState::InitGameData(const FString& InName)
{
    SetPlayerName(InName);
    HeadCard        = 0;
    HandCards.Empty();
    WinCount        = 0;
    bActionSelected = false;
    CurrentAction   = EPlayerAction::None;
    bSaved          = false;
}

int32 AIndianPokerPlayerState::GetHandTotal() const
{
    int32 Total = 0;
    for (int32 Card : HandCards) Total += Card;
    return Total;
}

int32 AIndianPokerPlayerState::GetTotalScore() const
{
    return HeadCard + GetHandTotal();
}

void AIndianPokerPlayerState::OnRep_GameData()
{
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World);
    if (!LocalPC) return;

    AIndianPokerPlayerController* IPC =
        Cast<AIndianPokerPlayerController>(LocalPC);
    if (!IPC) return;

    IPC->RefreshUI();
    IPC->RefreshHeadWidgets();
}

void AIndianPokerPlayerState::UpdateHeadWidget()
{
   
}