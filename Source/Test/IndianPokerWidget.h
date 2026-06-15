#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndianPokerPlayerState.h"
#include "IndianPokerWidget.generated.h"

UCLASS()
class TEST_API UIndianPokerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 내 손패
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* MyHandText;

    // 승리 횟수
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* MyWinCountText;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* EnemyWinCountText;

    // 턴
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* TurnText;

    // 상태 메시지
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* StatusText;

    // 행동 버튼
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* AttackButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* DefendButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UButton* SaveButton;

    UFUNCTION(BlueprintCallable)
    void UpdateUIFromData(
        const TArray<int32>& MyHandCards,
        int32 MyWinCount,
        int32 EnemyWinCount,
        int32 TurnNum);

    // 버튼 활성화
    UFUNCTION(BlueprintCallable)
    void SetButtonsEnabled(bool bEnabled);

    // 상태 메시지
    UFUNCTION(BlueprintCallable)
    void SetStatusMessage(const FString& Message);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnAttackClicked();

    UFUNCTION()
    void OnDefendClicked();

    UFUNCTION()
    void OnSaveClicked();
};