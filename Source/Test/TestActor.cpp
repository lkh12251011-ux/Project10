#include "TestActor.h"

ATestActor::ATestActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATestActor::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            5.f,
            FColor::Green,
            TEXT("TestActor::BeginPlay — Test 모듈 정상 작동!")
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("TestActor BeginPlay called!"));
}