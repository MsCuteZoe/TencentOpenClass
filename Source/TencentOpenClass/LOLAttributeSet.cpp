// Fill out your copyright notice in the Description page of Project Settings.


#include "LOLAttributeSet.h"

void ULOLAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void ULOLAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);

	
}
