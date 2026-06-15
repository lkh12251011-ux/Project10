#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IndianPokerPlayerState.h" 
#include "CardDeck.h"
#include "IndianPokerGameMode.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    WaitingForPlayers  UMETA(DisplayName = "플레이어 대기"),
    TurnStart          UMETA(DisplayName = "턴 시작"),
    WaitingForActions  UMETA(DisplayName = "행동 선택 중"),
    ResolveTurn        UMETA(DisplayName = "턴 결과 처리"),
    GameOver           UMETA(DisplayName = "게임 종료")
};

UCLASS()
class TEST_API AIndianPokerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AIndianPokerGameMode();

protected:
    virtual void BeginPlay() override;

    // 플레이어 접속 시 자동 호출
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // 플레이어 접속 해제 시
    virtual void Logout(AController* Exiting) override;

public:
    UPROPERTY()
    TArray<AIndianPokerPlayerState*> PlayerStates;

    UPROPERTY()
    TArray<class AIndianPokerPlayerController*> PlayerControllers;

    UPROPERTY()
    UCardDeck* HeadDeck;

    UPROPERTY()
    UCardDeck* HandDeck;

    UPROPERTY(BlueprintReadOnly, Category = "Game")
    EGamePhase CurrentPhase;

    // 최대 플레이어 수
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 MaxPlayers;

    // 승리 조건
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    int32 WinCondition;

    // 현재 턴
    UPROPERTY(BlueprintReadOnly, Category = "Game")
    int32 CurrentTurn;

    // 플레이어 행동 설정 PlayerController에서 RPC로 호출
    void SetPlayerAction(int32 PlayerIndex, EPlayerAction Action);

private:
    void StartTurn();
    void ResolveTurn();
    void ResolveOneVsOne(AIndianPokerPlayerState* P1, AIndianPokerPlayerState* P2);
    void AddWin(AIndianPokerPlayerState* Winner, AIndianPokerPlayerState* Loser,
        int32 WinnerIdx, int32 LoserIdx);
    bool CheckGameOver();

    // 특정 클라이언트에게 메시지
    void SendMessageToAll(const FString& Message);
};
