// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoinCatcherGameMode.h"
#include "CoinCatcherCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACoinCatcherGameMode::ACoinCatcherGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
