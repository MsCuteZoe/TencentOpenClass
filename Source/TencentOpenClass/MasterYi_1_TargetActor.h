// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "MasterYi_1_TargetActor.generated.h"

/**
 * 
 */
UCLASS()
class TENCENTOPENCLASS_API AMasterYi_1_TargetActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AMasterYi_1_TargetActor() {}

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void ConfirmTargetingAndContinue() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability Settings", meta=(ExposeOnSpawn=true))
	float Radius = 1000.f;
	
};
