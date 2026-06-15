#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CardDeck.generated.h"

UCLASS()
class TEST_API UCardDeck : public UObject
{
    GENERATED_BODY()

public:
    UCardDeck();

    // 덱 초기화 (1~10 한 그룹)
    void InitDeck();

    // 카드 한 장 뽑기 (-1이면 덱 소진)
    int32 DrawCard();

    // 남은 카드 수
    int32 RemainingCards() const;

    // 덱이 비었는지
    bool IsEmpty() const;

private:
    TArray<int32> Cards;

    void Shuffle();
};