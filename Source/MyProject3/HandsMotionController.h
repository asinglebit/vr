// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputCoreTypes.h"
#include "MotionControllerComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "HandsMotionController.generated.h"

UCLASS()
class MYPROJECT3_API AHandsMotionController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandsMotionController();

	UPROPERTY(Meta = (ExposeOnSpawn = "true"), EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TEnumAsByte<EControllerHand> EHand;

	USceneComponent* TeleportCylinder;
	USplineComponent* ArcSpline;
	USceneComponent* ArcEndPoint;
	UMotionControllerComponent* MotionController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TArray<USplineMeshComponent*> ASplineMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FTeleportLaunchVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsTeleporterActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsValidTeleportDestination;

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FClearArc();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FActivateTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FDisableTeleporter();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	FVector FGetTeleportDestination();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
