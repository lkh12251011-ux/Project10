#include "IndianPokerPlayerController.h"
#include "IndianPokerWidget.h"
#include "IndianPokerGameMode.h"
#include "IndianPokerGameState.h"
#include "IndianPokerPlayerState.h"
#include "Net/UnrealNetwork.h"

AIndianPokerPlayerController::AIndianPokerPlayerController()
{
}

void AIndianPokerPlayerController::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AIndianPokerPlayerController, PlayerIndex);
}

void AIndianPokerPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalController())
        CreateGameUI();
}

void AIndianPokerPlayerController::CreateGameUI()
{
    if (WidgetClass)
    {
        GameWidget = CreateWidget<UIndianPokerWidget>(this, WidgetClass);
        if (GameWidget)
        {
            GameWidget->AddToViewport();
            GameWidget->SetButtonsEnabled(false);
        }
    }
}

bool AIndianPokerPlayerController::ServerSetAction_Validate(EPlayerAction Action)
{
    return Action != EPlayerAction::None;
}

void AIndianPokerPlayerController::ServerSetAction_Implementation(
    EPlayerAction Action)
{
    AIndianPokerGameMode* GM =
        Cast<AIndianPokerGameMode>(GetWorld()->GetAuthGameMode());
    AIndianPokerPlayerState* PS =
        GetPlayerState<AIndianPokerPlayerState>();

    if (GM && PS && PS->PlayerIndex >= 0)
        GM->SetPlayerAction(PS->PlayerIndex, Action);
}

void AIndianPokerPlayerController::ClientShowMessage_Implementation(
    const FString& Message)
{
    if (GameWidget)
        GameWidget->SetStatusMessage(Message);
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, Message);
}

void AIndianPokerPlayerController::ClientSetButtonsEnabled_Implementation(
    bool bEnabled)
{
    if (GameWidget)
        GameWidget->SetButtonsEnabled(bEnabled);
}

void AIndianPokerPlayerController::RefreshUI()
{
    if (!GameWidget) return;

    AIndianPokerPlayerState* MyPS =
        GetPlayerState<AIndianPokerPlayerState>();
    if (!MyPS) return;

    int32 EnemyWin = 0;
    if (AGameStateBase* GS = GetWorld()->GetGameState())
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (PS == MyPS) continue;
            if (AIndianPokerPlayerState* E =
                Cast<AIndianPokerPlayerState>(PS))
                EnemyWin = E->WinCount;
        }
    }

    int32 Turn = 0;
    if (AIndianPokerGameState* GState =
        GetWorld()->GetGameState<AIndianPokerGameState>())
        Turn = GState->CurrentTurn;

    UE_LOG(LogTemp, Warning,
        TEXT("[클라] RefreshUI — 손패:%d장 내승리:%d 상대승리:%d 턴:%d 행동선택:%s"),
        MyPS->HandCards.Num(), MyPS->WinCount, EnemyWin, Turn,
        MyPS->bActionSelected ? TEXT("완료") : TEXT("미완료"));

    GameWidget->UpdateUIFromData(
        MyPS->HandCards, MyPS->WinCount, EnemyWin, Turn);

    GameWidget->SetButtonsEnabled(!MyPS->bActionSelected);
}

void AIndianPokerPlayerController::RefreshHeadWidgets()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AIndianPokerPlayerState* MyPS =
        GetPlayerState<AIndianPokerPlayerState>();

    if (AGameStateBase* GS = World->GetGameState())
    {
        for (APlayerState* PS : GS->PlayerArray)
        {
            AIndianPokerPlayerState* IPS =
                Cast<AIndianPokerPlayerState>(PS);
            if (!IPS) continue;

            APawn* OwnerPawn = IPS->GetPawn();  
            if (!OwnerPawn) continue;

            UFunction* Func =
                OwnerPawn->FindFunction(TEXT("UpdateHeadNumber"));
            if (!Func) continue;

            struct FParams { int32 Number; } Params;

            if (IPS == MyPS)
                Params.Number = -1;
            else
                Params.Number = IPS->HeadCard;

            OwnerPawn->ProcessEvent(Func, &Params);  

            UE_LOG(LogTemp, Warning,
                TEXT("[클라] %s 머리 위 숫자: %d"),
                *IPS->GetPlayerName(), Params.Number);
        }
    }
}