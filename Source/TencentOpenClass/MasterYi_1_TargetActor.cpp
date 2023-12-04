// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterYi_1_TargetActor.h"

#include "Abilities/GameplayAbility.h"

void AMasterYi_1_TargetActor::StartTargeting(UGameplayAbility* Ability) {
	Super::StartTargeting(Ability);

	OwningAbility = Ability;
	MasterPC = Cast<APlayerController>(Ability->GetOwningActorFromActorInfo()->GetInstigatorController());
	SourceActor = Ability->GetOwningActorFromActorInfo();
}

void AMasterYi_1_TargetActor::ConfirmTargetingAndContinue() {
	Super::ConfirmTargetingAndContinue();

	FVector AbilityStartLocation = FVector::ZeroVector;

	TArray<FOverlapResult> OverlapResults;
	TArray<TWeakObjectPtr<AActor>> OverlappedActors;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;
	if (SourceActor) {
		QueryParams.AddIgnoredActor(SourceActor->GetUniqueID());
		AbilityStartLocation = SourceActor->GetActorLocation();
	}

	// 构建球形碰撞体， 进行碰撞检测
	bool TryOverlap = GetWorld()->OverlapMultiByObjectType(OverlapResults,
		AbilityStartLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius),
		QueryParams);

	if (TryOverlap) {
		for (const FOverlapResult& Result : OverlapResults) {
			APawn* OverlappedPawn = Cast<APawn>(Result.GetActor());
			if (IsValid(OverlappedPawn)) {
				OverlappedActors.AddUnique(OverlappedPawn);
			}
		}
	}

	if (OverlappedActors.Num() > 0) {
		const FGameplayAbilityTargetDataHandle TargetData = StartLocation.MakeTargetDataHandleFromActors(OverlappedActors);
		TargetDataReadyDelegate.Broadcast(TargetData);
	}
	
}
