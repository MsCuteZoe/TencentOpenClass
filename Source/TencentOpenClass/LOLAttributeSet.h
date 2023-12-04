// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LOLAttributeSet.generated.h"



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class TENCENTOPENCLASS_API ULOLAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// Attributes
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Health);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, MaxHealth);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Mana);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, MaxMana);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Speed);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, AttackSpeed);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
