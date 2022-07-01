// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheLastOfUSGameMode.h"
#include "TheLastOfUSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheLastOfUSGameMode::ATheLastOfUSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/Blueprint/Player/PlayerCharacter.PlayerCharacter_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
