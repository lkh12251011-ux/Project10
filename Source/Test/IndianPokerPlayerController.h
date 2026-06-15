#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IndianPokerPlayerState.h"
#include "IndianPokerPlayerController.generated.h"

UCLASS()
class TEST_API AIndianPokerPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AIndianPokerPlayerController();

protected:
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UIndianPokerWidget> WidgetClass;

    UPROPERTY()
    class UIndianPokerWidget* GameWidget;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
    int32 PlayerIndex;

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetAction(EPlayerAction Action);

    UFUNCTION(Client, Reliable)
    void ClientShowMessage(const FString& Message);

    UFUNCTION(Client, Reliable)
    void ClientSetButtonsEnabled(bool bEnabled);

    void CreateGameUI();

    void RefreshUI();

    void RefreshHeadWidgets();
};