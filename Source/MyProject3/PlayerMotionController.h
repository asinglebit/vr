// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HandsMotionController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HandsMotionController.h"
#include "PlayerMotionController.generated.h"

UCLASS()
class MYPROJECT3_API APlayerMotionController : public APawn
{
	GENERATED_BODY()

	virtual void PostInitializeComponents() override;

public:
	APlayerMotionController();

	UCapsuleComponent* ACapsule;
	UCameraComponent* ACamera;
	USphereComponent* CameraCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	FVector VLastRoomScalePosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	FVector VLastCapsuleLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsTeleporting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsLeftStickDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsCameraOverlapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsFalling;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	bool BIsCapsuleHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FCameraCollidedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FMovementMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FLastCapsuleZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FMaxStepHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FMaxFloorSlope;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FEyeHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FRealignDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FFadeInDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	float FFadeOutDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	FLinearColor CTeleportFadeColor;

	USceneComponent* VROrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	TSubclassOf<AActor> BPMotionControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	AHandsMotionController* ALeftController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Exposed)
	AHandsMotionController* ARightController;

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateCapsuleHeight();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateActorPosition();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FInitVariables();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateVariables();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FCheckCapsuleSeparation();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FUpdateRoomScalePosition();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FExecuteTeleportation(AHandsMotionController* MotionController);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FTrackPadMovement();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FCheckUpdateActorPosition();

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	bool FIsUnderMaxStepHeight(bool Flag, FVector InVector);

	UFUNCTION(BlueprintCallable, Category = "Exposed")
	FVector FGetCapsuleBottom();
	
	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FCheckFloor();
	
	UFUNCTION(BlueprintCallable, Category = "Exposed")
	void FCheckCameraOverlap();
	
	UFUNCTION()
	void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TeleportRightPressed();
	void TeleportRightReleased();
	void TeleportLeftPressed();
	void TeleportLeftReleased();
	void GrabRightPressed();
	void GrabRightReleased();
	void GrabLeftPressed();
	void GrabLeftReleased();

	FTimerHandle TeleportationTimerHandle;
	UFUNCTION()
	void FExecuteTeleportationDelayed(AHandsMotionController* MotionController);

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//void MotionControllerThumbLeft_Y(float AxisValue);

	static const FName MotionControllerThumbLeftXBinding;
	static const FName MotionControllerThumbLeftYBinding;
};
