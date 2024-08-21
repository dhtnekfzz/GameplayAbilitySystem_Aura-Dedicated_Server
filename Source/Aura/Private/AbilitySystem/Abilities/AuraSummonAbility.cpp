// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	const FVector Forward=GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location=GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread=SpawnSpread/NumMinions;

	const FVector LeftSpread=Forward.RotateAngleAxis(-SpawnSpread/2, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for(int32 i=0; i<NumMinions; i++)
	{
		const FVector Direction=LeftSpread.RotateAngleAxis(DeltaSpread*i, FVector::UpVector);
		FVector SpawnLocation=Location+Direction*FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation+FVector(0.f,0.f,400.f), SpawnLocation-FVector(0.f,0.f,400.f),ECC_Visibility);
		if(Hit.bBlockingHit)
		{
			SpawnLocation=Hit.ImpactPoint;
		}
		
		SpawnLocations.Add(SpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection=FMath::RandRange(0, MinionClasses.Num()-1);
	return MinionClasses[Selection];
}
