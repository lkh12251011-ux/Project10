#include "IndianPokerGameMode.h"
#include "IndianPokerPlayerController.h"
#include "IndianPokerPlayerState.h"
#include "IndianPokerGameState.h"

AIndianPokerGameMode::AIndianPokerGameMode()
{
    MaxPlayers = 2;
    WinCondition = 5;
    CurrentPhase = EGamePhase::WaitingForPlayers;

    PlayerControllerClass = AIndianPokerPlayerController::StaticClass();
    PlayerStateClass = AIndianPokerPlayerState::StaticClass();
    GameStateClass = AIndianPokerGameState::StaticClass();
}

void AIndianPokerGameMode::BeginPlay()
{
    Super::BeginPlay();

    HeadDeck = NewObject<UCardDeck>(this);
    HandDeck = NewObject<UCardDeck>(this);
    HeadDeck->InitDeck();
    HandDeck->InitDeck();

    UE_LOG(LogTemp, Warning, TEXT("[서버] 게임 모드 초기화 완료. 플레이어 대기 중..."));
}

void AIndianPokerGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    AIndianPokerPlayerController* PC =
        Cast<AIndianPokerPlayerController>(NewPlayer);
    if (!PC) return;

    // PlayerState 가져오기
    AIndianPokerPlayerState* PS =
        NewPlayer->GetPlayerState<AIndianPokerPlayerState>();
    if (!PS) return;

    PS->PlayerIndex = PlayerStates.Num();
    PS->InitGameData(
        FString::Printf(TEXT("Player%d"), PlayerStates.Num() + 1));

    PlayerStates.Add(PS);
    PlayerControllers.Add(PC);

    UE_LOG(LogTemp, Warning,
        TEXT("[서버] Player%d 접속! (%d/%d)"),
        PS->PlayerIndex, PlayerStates.Num(), MaxPlayers);

    SendMessageToAll(FString::Printf(
        TEXT("Player%d 접속! (%d/%d)"),
        PS->PlayerIndex + 1, PlayerStates.Num(), MaxPlayers));

    if (PlayerStates.Num() >= MaxPlayers)
        StartTurn();
}

void AIndianPokerGameMode::StartTurn()
{
    CurrentTurn++;
    CurrentPhase = EGamePhase::TurnStart; 

    if (AIndianPokerGameState* GS =
        GetWorld()->GetGameState<AIndianPokerGameState>())
        GS->CurrentTurn = CurrentTurn;

    UE_LOG(LogTemp, Warning, TEXT("[서버] === 턴 %d 시작 ==="), CurrentTurn);

    if (HeadDeck->RemainingCards() < PlayerStates.Num())
        HeadDeck->InitDeck();
    if (HandDeck->RemainingCards() < PlayerStates.Num() * 2)
        HandDeck->InitDeck();

    for (AIndianPokerPlayerState* PS : PlayerStates)
    {
        if (!PS) continue;

        PS->HeadCard = HeadDeck->DrawCard();

        if (!PS->bSaved)
        {
            PS->HandCards.Empty();
            PS->HandCards.Add(HandDeck->DrawCard());
        }
        else
        {
            PS->HandCards.Add(HandDeck->DrawCard());
            PS->bSaved = false;
        }

        PS->CurrentAction = EPlayerAction::None;
        PS->bActionSelected = false;

        UE_LOG(LogTemp, Warning,
            TEXT("[서버] %s HeadCard=%d HandCards=%d장"),
            *PS->GetPlayerName(), PS->HeadCard, PS->HandCards.Num());
    }

    CurrentPhase = EGamePhase::WaitingForActions;

    SendMessageToAll(
        FString::Printf(TEXT("턴 %d — 행동을 선택하세요!"), CurrentTurn));
}

void AIndianPokerGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    AIndianPokerPlayerController* PC = Cast<AIndianPokerPlayerController>(Exiting);
    if (!PC) return;

    AIndianPokerPlayerState* PS = PC->GetPlayerState<AIndianPokerPlayerState>();
    int32 Idx = PS ? PS->PlayerIndex : -1;

    SendMessageToAll(FString::Printf(TEXT("Player%d 접속 해제"), Idx + 1));
    UE_LOG(LogTemp, Warning, TEXT("[서버] 플레이어 접속 해제"));
}

void AIndianPokerGameMode::SetPlayerAction(int32 PlayerIndex, EPlayerAction Action)
{
    if (!PlayerStates.IsValidIndex(PlayerIndex)) return;
    if (CurrentPhase != EGamePhase::WaitingForActions) return;

    AIndianPokerPlayerState* Player = PlayerStates[PlayerIndex];

    // 이미 선택했으면 무시
    if (Player->bActionSelected)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[서버] Player%d 이미 행동 선택함"), PlayerIndex);
        return;
    }

    Player->CurrentAction = Action;
    Player->bActionSelected = true; 

    UE_LOG(LogTemp, Warning,
        TEXT("[서버] Player%d 행동 선택: %s"),
        PlayerIndex,
        *UEnum::GetValueAsString(Action)
    );

    // 일회성 안내 메시지만 RPC로
    if (PlayerControllers.IsValidIndex(PlayerIndex))
    {
        PlayerControllers[PlayerIndex]->ClientShowMessage(TEXT("행동 선택 완료! 상대 대기 중..."));
    }

    // 모든 플레이어가 행동 선택 완료 시 턴 결과 처리
    bool bAllSelected = true;
    for (AIndianPokerPlayerState* P : PlayerStates)
    {
        if (!P->bActionSelected)
        {
            bAllSelected = false;
            break;
        }
    }

    if (bAllSelected)
        ResolveTurn();
}

void AIndianPokerGameMode::ResolveTurn()
{
    CurrentPhase = EGamePhase::ResolveTurn;

    AIndianPokerGameState* GState = GetGameState<AIndianPokerGameState>();
    int32 Turn = GState ? GState->CurrentTurn : 0;
    UE_LOG(LogTemp, Warning, TEXT("[서버] === 턴 %d 결과 처리 ==="), Turn);

    if (PlayerStates.Num() == 2)
        ResolveOneVsOne(PlayerStates[0], PlayerStates[1]);

    if (!CheckGameOver())
        StartTurn();
}

void AIndianPokerGameMode::ResolveOneVsOne(AIndianPokerPlayerState* P1, AIndianPokerPlayerState* P2)
{
    EPlayerAction A1 = P1->CurrentAction;
    EPlayerAction A2 = P2->CurrentAction;

    FString ResultMsg;

    // 공격 vs 공격
    if (A1 == EPlayerAction::Attack && A2 == EPlayerAction::Attack)
    {
        if (P1->GetTotalScore() > P2->GetTotalScore())
        {
            AddWin(P1, P2, 0, 1);
            ResultMsg = FString::Printf(TEXT("%s 승리! (총점 %d vs %d)"),
                *P1->GetPlayerName(), P1->GetTotalScore(), P2->GetTotalScore());
        }
        else if (P2->GetTotalScore() > P1->GetTotalScore())
        {
            AddWin(P2, P1, 1, 0);
            ResultMsg = FString::Printf(TEXT("%s 승리! (총점 %d vs %d)"),
                *P2->GetPlayerName(), P2->GetTotalScore(), P1->GetTotalScore());
        }
        else
        {
            ResultMsg = TEXT("동점! 무승부");
        }
    }
    // 공격 vs 방어
    else if ((A1 == EPlayerAction::Attack && A2 == EPlayerAction::Defend) ||
        (A1 == EPlayerAction::Defend && A2 == EPlayerAction::Attack))
    {
        AIndianPokerPlayerState* Attacker = (A1 == EPlayerAction::Attack) ? P1 : P2;
        AIndianPokerPlayerState* Defender = (A1 == EPlayerAction::Defend) ? P1 : P2;
        int32 AttIdx = (A1 == EPlayerAction::Attack) ? 0 : 1;
        int32 DefIdx = (A1 == EPlayerAction::Defend) ? 0 : 1;

        if (Attacker->GetHandTotal() > Defender->GetHandTotal())
        {
            AddWin(Attacker, Defender, AttIdx, DefIdx);
            ResultMsg = FString::Printf(TEXT("%s 승리! (손패 %d vs %d)"),
                *Attacker->GetPlayerName(),
                Attacker->GetHandTotal(), Defender->GetHandTotal());
        }
        else if (Defender->GetHandTotal() > Attacker->GetHandTotal())
        {
            AddWin(Defender, Attacker, DefIdx, AttIdx);
            ResultMsg = FString::Printf(TEXT("%s 승리! (손패 %d vs %d)"),
                *Defender->GetPlayerName(),
                Defender->GetHandTotal(), Attacker->GetHandTotal());
        }
        else
        {
            ResultMsg = TEXT("동점! 무승부");
        }
    }
    // 공격 vs 세이브 → 세이브 무조건 패
    else if ((A1 == EPlayerAction::Attack && A2 == EPlayerAction::Save) ||
        (A1 == EPlayerAction::Save && A2 == EPlayerAction::Attack))
    {
        AIndianPokerPlayerState* Attacker = (A1 == EPlayerAction::Attack) ? P1 : P2;
        AIndianPokerPlayerState* Saver = (A1 == EPlayerAction::Save) ? P1 : P2;
        int32 AttIdx = (A1 == EPlayerAction::Attack) ? 0 : 1;
        int32 SaveIdx = (A1 == EPlayerAction::Save) ? 0 : 1;

        AddWin(Attacker, Saver, AttIdx, SaveIdx);
        Saver->bSaved = false;
        ResultMsg = FString::Printf(TEXT("%s 승리! (공격 vs 세이브)"),
            *Attacker->GetPlayerName());
    }
    // 방어 vs 세이브 → 무승부, 세이브 효과 적용
    else if ((A1 == EPlayerAction::Defend && A2 == EPlayerAction::Save) ||
        (A1 == EPlayerAction::Save && A2 == EPlayerAction::Defend))
    {
        AIndianPokerPlayerState* Saver = (A1 == EPlayerAction::Save) ? P1 : P2;
        Saver->bSaved = true;
        ResultMsg = FString::Printf(
            TEXT("무승부! %s 다음 턴 손패 2장"), *Saver->GetPlayerName());
    }
    // 세이브 vs 세이브
    else if (A1 == EPlayerAction::Save && A2 == EPlayerAction::Save)
    {
        P1->bSaved = true;
        P2->bSaved = true;
        ResultMsg = TEXT("양측 세이브! 둘 다 다음 턴 손패 2장");
    }
    // 방어 vs 방어
    else
    {
        ResultMsg = TEXT("무승부");
    }

    UE_LOG(LogTemp, Warning, TEXT("[서버] 결과: %s"), *ResultMsg);
    SendMessageToAll(ResultMsg);
}

void AIndianPokerGameMode::AddWin(AIndianPokerPlayerState* Winner, AIndianPokerPlayerState* Loser,
    int32 WinnerIdx, int32 LoserIdx)
{
    Winner->WinCount++;
    UE_LOG(LogTemp, Warning,
        TEXT("[서버] %s 승리! (%d승)"), *Winner->GetPlayerName(), Winner->WinCount);
}

bool AIndianPokerGameMode::CheckGameOver()
{
    for (int32 i = 0; i < PlayerStates.Num(); i++)
    {
        if (PlayerStates[i]->WinCount >= WinCondition)
        {
            CurrentPhase = EGamePhase::GameOver;

            FString WinMsg = FString::Printf(
                TEXT("게임 종료! %s 최종 승리! (%d승)"),
                *PlayerStates[i]->GetPlayerName(), PlayerStates[i]->WinCount
            );

            UE_LOG(LogTemp, Warning, TEXT("[서버] %s"), *WinMsg);
            SendMessageToAll(WinMsg);
            return true;
        }
    }
    return false;
}

void AIndianPokerGameMode::SendMessageToAll(const FString& Message)
{
    for (AIndianPokerPlayerController* PC : PlayerControllers)
    {
        if (PC) PC->ClientShowMessage(Message);
    }
}