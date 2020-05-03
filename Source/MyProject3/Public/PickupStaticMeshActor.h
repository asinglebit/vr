// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "../HandsMotionController.h"
#include "PickupStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT3_API APickupStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;
	
public:

	APickupStaticMeshActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TSubclassOf<AActor> BPMotionControllerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TArray<AActor*> AActorsToIgnore;

protected:

	virtual void BeginPlay() override;
};
