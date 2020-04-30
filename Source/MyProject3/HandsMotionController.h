// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineUtils.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputCoreTypes.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Public/PickupActor.h"
#include "HandsMotionController.generated.h"

UCLASS()
class MYPROJECT3_API AHandsMotionController : public AActor
{
	GENERATED_BODY()

	UStaticMesh* Mesh;
	UMaterialInterface* Material;

	virtual void PostInitializeComponents() override;

public:	
	// Sets default values for this actor's properties
	AHandsMotionController();

	UPROPERTY(Meta = (ExposeOnSpawn = "true"), EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TEnumAsByte<EControllerHand> EHand;

	USceneComponent* TeleportCylinder;
	USplineComponent* ArcSpline;
	USceneComponent* ArcEndPoint;
	USphereComponent* GrabSphere;
	UMotionControllerComponent* AMotionController;
	UPhysicsHandleComponent* APhysicsHandle;

	bool BIsToBeGripped;
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TArray<USplineMeshComponent*> ASplineMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FTeleportLaunchVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsTeleporterActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsValidTeleportDestination; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BLastFrameValidDestination;

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FClearArc();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FActivateTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FDisableTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	AActor* FGetActorNearHand();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	FVector FGetTeleportDestination();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateArcEndpoint(bool IsValidLocationFound, FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateArcSpline(bool IsValidLocationFound, TArray<FVector> SplinePoints);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	bool FTraceTeleportDestination(TArray<FVector> & TracePoints, FVector & NavMeshLocation, FVector & TraceLocation);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FGrabActor();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FReleaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
