#include "CardDeck.h"

UCardDeck::UCardDeck()
{
    InitDeck();
}

void UCardDeck::InitDeck()
{
    Cards.Empty();
    // 1~10 한 그룹
    for (int32 i = 1; i <= 10; i++)
    {
        Cards.Add(i);
    }
    Shuffle();
}

void UCardDeck::Shuffle()
{
    for (int32 i = Cards.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        Cards.Swap(i, j);
    }
}

int32 UCardDeck::DrawCard()
{
    if (Cards.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("덱이 비었습니다! 덱을 재초기화합니다."));
        InitDeck();
    }
    int32 Card = Cards.Last();
    Cards.RemoveAt(Cards.Num() - 1);
    return Card;
}

int32 UCardDeck::RemainingCards() const
{
    return Cards.Num();
}

bool UCardDeck::IsEmpty() const
{
    return Cards.Num() == 0;
}