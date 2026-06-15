#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IndianPokerPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerAction : uint8
{
    None    UMETA(DisplayName = "없음"),
    Attack  UMETA(DisplayName = "공격"),
    Defend  UMETA(DisplayName = "방어"),
    Save    UMETA(DisplayName = "세이브")
};

UCLASS()
class TEST_API AIndianPokerPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AIndianPokerPlayerState();

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

   
    UPROPERTY(ReplicatedUsing = OnRep_GameData, BlueprintReadOnly, Category = "Game")
    int32 HeadCard = 0;

    UPROPERTY(ReplicatedUsing = OnRep_GameData, BlueprintReadOnly, Category = "Game")
    TArray<int32> HandCards;

    UPROPERTY(ReplicatedUsing = OnRep_GameData, BlueprintReadOnly, Category = "Game")
    int32 WinCount = 0;

    UPROPERTY(ReplicatedUsing = OnRep_GameData, BlueprintReadOnly, Category = "Game")
    bool bActionSelected = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    EPlayerAction CurrentAction = EPlayerAction::None;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    bool bSaved = false;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    int32 PlayerIndex = -1;

    void InitGameData(const FString& InName);
    int32 GetHandTotal() const;
    int32 GetTotalScore() const;

protected:
    UFUNCTION()
    void OnRep_GameData();

    void UpdateHeadWidget();
};