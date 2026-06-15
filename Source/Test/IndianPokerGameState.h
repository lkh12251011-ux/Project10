#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IndianPokerGameState.generated.h"

UCLASS()
class TEST_API AIndianPokerGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(ReplicatedUsing = OnRep_Turn, BlueprintReadOnly, Category = "Game")
    int32 CurrentTurn = 0;

protected:
    UFUNCTION()
    void OnRep_Turn();
};