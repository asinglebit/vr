// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "EnumGrabMethod.h"
#include "PickupActor.h"
#include "../HandsMotionController.h"
#include "PickupStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT3_API APickupStaticMeshActor : public AStaticMeshActor, public IPickupActor
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;
	
public:

	APickupStaticMeshActor();

	UPROPERTY(Meta = (ExposeOnSpawn = "true"), EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TEnumAsByte<EnumGrabMethod> EGrabMethod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	USceneComponent* AMotionController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	UPhysicsHandleComponent* APhysicsHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsHitValid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsMixedModeGrabbed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FHitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FLocationAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	FTransform TRelativePickupOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TSubclassOf<AActor> BPMotionControllerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TArray<AActor*> AActorsToIgnore;

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FDropIfLostConnection();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FDropPhysicsHandle();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FDropAttachTo();

protected:

	virtual void BeginPlay() override;
};
