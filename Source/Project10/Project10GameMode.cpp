// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project10GameMode.h"
#include "Project10Character.h"
#include "UObject/ConstructorHelpers.h"

AProject10GameMode::AProject10GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
