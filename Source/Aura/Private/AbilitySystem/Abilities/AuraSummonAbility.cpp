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
		const FVector SpawnLocation=Location+Direction*FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocations.Add(SpawnLocation);
	}

	return SpawnLocations;
}
