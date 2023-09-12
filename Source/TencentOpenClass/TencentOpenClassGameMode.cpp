// Copyright Epic Games, Inc. All Rights Reserved.

#include "TencentOpenClassGameMode.h"
#include "TencentOpenClassCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATencentOpenClassGameMode::ATencentOpenClassGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
