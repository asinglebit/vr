#include "PlayerMotionController.h"

#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"

// Sets default values
APlayerMotionController::APlayerMotionController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<AActor> ActorClassFinder(TEXT("/Game/Blueprints/BP_MotionController"));
	this->BPMotionControllerClass = ActorClassFinder.Class;

	this->FEyeHeightOffset = 7.0f;
	this->FFadeInDuration = 0.2f;
	this->FFadeOutDuration = 0.1f;
	this->CTeleportFadeColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void APlayerMotionController::PostInitializeComponents() {
	Super::PostInitializeComponents();
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	// Get components
	auto components = this->GetComponents();
	for (auto component : components)
	{
		const FName ComponentName = component->GetFName();
		if (ComponentName == "VROrigin") {
			this->VROrigin = CastChecked<USceneComponent>(component);
		} else if (ComponentName == "Capsule") {
			this->ACapsule = CastChecked<UCapsuleComponent>(component);
		}
		else if (ComponentName == "Camera") {
			this->ACamera = CastChecked<UCameraComponent>(component);
		}
	}
	
	// Spawn left hand
	this->ALeftController = GetWorld()->SpawnActorDeferred<AHandsMotionController>(this->BPMotionControllerClass, FTransform::Identity, this, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	this->ALeftController->EHand = EControllerHand::Left;
	UGameplayStatics::FinishSpawningActor(this->ALeftController, FTransform::Identity);
	this->ALeftController->AttachToComponent(this->VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));

	// Spawn right hand
	this->ARightController = GetWorld()->SpawnActorDeferred<AHandsMotionController>(this->BPMotionControllerClass, FTransform::Identity, this, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	this->ARightController->EHand = EControllerHand::Right;
	UGameplayStatics::FinishSpawningActor(this->ARightController, FTransform::Identity);
	this->ARightController->AttachToComponent(this->VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
}

void APlayerMotionController::FUpdateCapsuleHeight()
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
	const float CapsuleEyeOffset = DevicePosition.Z + this->FEyeHeightOffset;
	const float DeltaLocationZ = CapsuleEyeOffset - this->FLastCapsuleZ;
	const FVector DeltaLocation = FVector(0.0f, 0.0f, DeltaLocationZ);
	this->ACapsule->AddWorldOffset(DeltaLocation);
	this->ACapsule->SetCapsuleHalfHeight(CapsuleEyeOffset / 2.0f, true);
	this->FLastCapsuleZ = CapsuleEyeOffset;
}

void APlayerMotionController::FUpdateActorPosition()
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
	const FVector CapsuleLocation = this->ACapsule->GetComponentLocation();
	const float CapsuleHalfHeight = this->ACapsule->GetScaledCapsuleHalfHeight();
	const FVector CorrectedHMDLocation = FVector(DevicePosition.X, DevicePosition.Y, CapsuleHalfHeight);
	this->SetActorLocation(CapsuleLocation - CorrectedHMDLocation);
}

void APlayerMotionController::FInitVariables()
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
	this->FLastCapsuleZ = DevicePosition.Z + this->FEyeHeightOffset;
	this->FUpdateCapsuleHeight();
	this->VLastRoomScalePosition = FVector(DevicePosition.X, DevicePosition.Y, 0.0f);

	const FVector CameraLocation = this->ACamera->GetComponentLocation();
	const FVector NewLocation = FVector(CameraLocation.X, CameraLocation.Y, this->ACapsule->GetScaledCapsuleHalfHeight() + this->GetActorLocation().Z);
	this->ACapsule->SetWorldLocationAndRotation(NewLocation, FQuat::Identity);
	this->FUpdateActorPosition();
}

void APlayerMotionController::FUpdateVariables()
{
	this->VLastCapsuleLocation = this->ACapsule->GetComponentLocation();
}

void APlayerMotionController::FCheckCapsuleSeparation()
{
	const FVector VectorDifference = this->ACamera->GetComponentLocation() - this->ACapsule->GetComponentLocation();
	const float Difference = FVector(VectorDifference.X, VectorDifference.Y, 0.0f).Size();
	if (Difference > 60.0f) {
		this->FUpdateActorPosition();
	}
}

void APlayerMotionController::FUpdateRoomScalePosition()
{
	FRotator DeviceRotation;
	FVector DevicePosition;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
	FVector GroundedDevicePosition = FVector(DevicePosition.X, DevicePosition.Y, 0.0f);
	this->ACapsule->AddWorldOffset(GroundedDevicePosition - this->VLastRoomScalePosition);
	this->VLastRoomScalePosition = GroundedDevicePosition;
}

void APlayerMotionController::BeginPlay()
{
	Super::BeginPlay();

	this->FInitVariables();
}

void APlayerMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerMotionController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
