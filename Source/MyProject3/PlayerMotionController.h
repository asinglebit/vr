// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "HandsMotionController.h"
#include "PlayerMotionController.generated.h"

UCLASS()
class MYPROJECT3_API APlayerMotionController : public APawn
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

public:
	// Sets default values for this pawn's properties
	APlayerMotionController();

	USceneComponent* VROrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TSubclassOf<AActor> BPMotionControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	AHandsMotionController* ALeftController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	AHandsMotionController* ARightController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
