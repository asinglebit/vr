// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PickupActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT3_API IPickupActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interfaces")
	void FPickup(USceneComponent* MotionController, UPhysicsHandleComponent* PhysicsHandler);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interfaces")
	void FDrop();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interfaces")
	bool FIsHeldByMe(USceneComponent* MotionController);
};
