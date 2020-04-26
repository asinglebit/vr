// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputCoreTypes.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TArray<USplineMeshComponent*> ASplineMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsTeleporterActive;

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FClearArc();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
