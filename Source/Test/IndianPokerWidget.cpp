#include "IndianPokerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "IndianPokerPlayerController.h"

void UIndianPokerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (AttackButton)
        AttackButton->OnClicked.AddDynamic(
            this, &UIndianPokerWidget::OnAttackClicked);
    if (DefendButton)
        DefendButton->OnClicked.AddDynamic(
            this, &UIndianPokerWidget::OnDefendClicked);
    if (SaveButton)
        SaveButton->OnClicked.AddDynamic(
            this, &UIndianPokerWidget::OnSaveClicked);
}

void UIndianPokerWidget::UpdateUIFromData(
    const TArray<int32>& MyHandCards,
    int32 MyWinCount,
    int32 EnemyWinCount,
    int32 TurnNum)
{
    if (MyHandText)
    {
        FString HandStr = TEXT("내 손패: ");
        for (int32 Card : MyHandCards)
            HandStr += FString::Printf(TEXT("[%d] "), Card);
        MyHandText->SetText(FText::FromString(HandStr));
    }

    if (MyWinCountText)
        MyWinCountText->SetText(FText::FromString(
            FString::Printf(TEXT("내 승리: %d / 5"), MyWinCount)));

    if (EnemyWinCountText)
        EnemyWinCountText->SetText(FText::FromString(
            FString::Printf(TEXT("상대 승리: %d / 5"), EnemyWinCount)));

    if (TurnText)
        TurnText->SetText(FText::FromString(
            FString::Printf(TEXT("턴 %d"), TurnNum)));
}

void UIndianPokerWidget::SetButtonsEnabled(bool bEnabled)
{
    if (AttackButton) AttackButton->SetIsEnabled(bEnabled);
    if (DefendButton) DefendButton->SetIsEnabled(bEnabled);
    if (SaveButton)   SaveButton->SetIsEnabled(bEnabled);
}

void UIndianPokerWidget::SetStatusMessage(const FString& Message)
{
    if (StatusText)
        StatusText->SetText(FText::FromString(Message));
}

void UIndianPokerWidget::OnAttackClicked()
{
    AIndianPokerPlayerController* PC =
        Cast<AIndianPokerPlayerController>(GetOwningPlayer());
    if (PC) PC->ServerSetAction(EPlayerAction::Attack);
}

void UIndianPokerWidget::OnDefendClicked()
{
    AIndianPokerPlayerController* PC =
        Cast<AIndianPokerPlayerController>(GetOwningPlayer());
    if (PC) PC->ServerSetAction(EPlayerAction::Defend);
}

void UIndianPokerWidget::OnSaveClicked()
{
    AIndianPokerPlayerController* PC =
        Cast<AIndianPokerPlayerController>(GetOwningPlayer());
    if (PC) PC->ServerSetAction(EPlayerAction::Save);
}