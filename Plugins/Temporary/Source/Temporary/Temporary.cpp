#include "Temporary.h"

void FTemporaryModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("Temporary 플러그인 StartupModule 호출됨!"));
}

void FTemporaryModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("Temporary 플러그인 ShutdownModule 호출됨"));
}

IMPLEMENT_MODULE(FTemporaryModule, Temporary);