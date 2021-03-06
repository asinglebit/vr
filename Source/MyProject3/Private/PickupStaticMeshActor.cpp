#include "../Public/PickupStaticMeshActor.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

APickupStaticMeshActor::APickupStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;
	//PrimaryActorTick.bTickEvenWhenPaused = true;
	//PrimaryActorTick.TickGroup = TG_PrePhysics;

	static ConstructorHelpers::FClassFinder<AHandsMotionController> ActorClassFinder(TEXT("/Game/Blueprints/BP_MotionController"));
	BPMotionControllerClass = ActorClassFinder.Class;

	EGrabMethod = EnumGrabMethod::Mixed;
	IsGateAOpen = true;
	IsGateBOpen = true;
}

void APickupStaticMeshActor::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void APickupStaticMeshActor::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	BIsHitValid = true;
	for (auto actor : AActorsToIgnore) {
		if (actor == OtherActor) {
			BIsMixedModeGrabbed = false;
		}
	}
	FHitTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
}

void APickupStaticMeshActor::FDropIfLostConnection()
{
	if (!IsValid(AMotionController)) return;

	float Difference = (GetStaticMeshComponent()->GetComponentLocation() - AMotionController->GetComponentLocation()).Size();
	if (Difference > 30.0f) {
		Execute_FDrop(this);
	}
}

void APickupStaticMeshActor::FDropPhysicsHandle()
{
	APhysicsHandle->ReleaseComponent();
	GetStaticMeshComponent()->SetEnableGravity(true);
}

void APickupStaticMeshActor::FDropAttachTo()
{
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));
}

void APickupStaticMeshActor::FDropMixedMode()
{
	BIsMixedModeGrabbed = false;
	FDropPhysicsHandle();
	FDropAttachTo();
	GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	FLocationAlpha = 0.0f;
}

FTransform APickupStaticMeshActor::FGetWorldPickupTransform()
{
	FTransform MotionControllerTransform = AMotionController->GetComponentTransform();
	FVector VX;
	FVector VY;
	FVector VZ;
	UKismetMathLibrary::BreakRotIntoAxes(TRelativePickupOffset.Rotator(), VX, VY, VZ);
	return FTransform(
		UKismetMathLibrary::MakeRotationFromAxes(
			UKismetMathLibrary::TransformDirection(MotionControllerTransform, VX),
			UKismetMathLibrary::TransformDirection(MotionControllerTransform, VY),
			UKismetMathLibrary::TransformDirection(MotionControllerTransform, VZ)
		),
		UKismetMathLibrary::TransformLocation(MotionControllerTransform, TRelativePickupOffset.GetLocation())
	);
}

FTransform APickupStaticMeshActor::FGetRelativePickupTransform()
{
	FTransform MotionControllerTransform = AMotionController->GetComponentTransform();
	FVector VX;
	FVector VY;
	FVector VZ;
	UKismetMathLibrary::BreakRotIntoAxes(GetActorRotation(), VX, VY, VZ);
	return FTransform(
		UKismetMathLibrary::MakeRotationFromAxes(
			UKismetMathLibrary::InverseTransformDirection(MotionControllerTransform, VX),
			UKismetMathLibrary::InverseTransformDirection(MotionControllerTransform, VY),
			UKismetMathLibrary::InverseTransformDirection(MotionControllerTransform, VZ)
		),
		UKismetMathLibrary::InverseTransformLocation(MotionControllerTransform, GetActorLocation())
	);
}

void APickupStaticMeshActor::FGrabAttachTo()
{
	GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
	GetStaticMeshComponent()->SetEnableGravity(false);
	GetStaticMeshComponent()->SetSimulatePhysics(false);
	if (BIsMixedModeGrabbed) {
		FTransform Transform = FGetWorldPickupTransform();
		GetStaticMeshComponent()->SetWorldLocationAndRotation(
			Transform.GetLocation(),
			Transform.GetRotation()
		);
	}
	K2_GetRootComponent()->AttachToComponent(AMotionController, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));
	if (!BIsMixedModeGrabbed) {
		TRelativePickupOffset = FGetRelativePickupTransform();
	}
}

void APickupStaticMeshActor::FGrabPhysicsHandle()
{
	FTransform MotionControllerTransform = AMotionController->GetComponentTransform();
	APhysicsHandle->GrabComponentAtLocationWithRotation(
		GetStaticMeshComponent(),
		FName(),
		MotionControllerTransform.GetLocation(),
		MotionControllerTransform.Rotator()
	);
	GetStaticMeshComponent()->SetEnableGravity(false);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	FLocationAlpha = 0.0f;
}

void APickupStaticMeshActor::FGrabMixedMode()
{
	GetStaticMeshComponent()->SetCollisionResponseToChannel(
		ECollisionChannel::ECC_Pawn,
		ECollisionResponse::ECR_Ignore
	);
	FGrabAttachTo();
	BIsMixedModeGrabbed = true;
	IsGateAOpen = true;
	IsGateBOpen = true;
}

void APickupStaticMeshActor::FPickup_Implementation(USceneComponent* MotionController, UPhysicsHandleComponent* PhysicsHandle)
{
	if (IsValid(AMotionController)) {
		IPickupActor::Execute_FDrop(this);
	}
	AMotionController = MotionController;
	APhysicsHandle = PhysicsHandle;
	switch (EGrabMethod) {
	case EnumGrabMethod::AttachedTo:
		FGrabAttachTo();
		break;
	case EnumGrabMethod::PhysicalHandle:
		FGrabPhysicsHandle();
		break;
	case EnumGrabMethod::Mixed:
		FGrabMixedMode();
		break;
	}
}

void APickupStaticMeshActor::FDrop_Implementation()
{
	switch (EGrabMethod) {
	case EnumGrabMethod::AttachedTo:
		FDropAttachTo();
		break;
	case EnumGrabMethod::PhysicalHandle:
		FDropPhysicsHandle();
		break;
	case EnumGrabMethod::Mixed:
		FDropMixedMode();
		break;
	}
	AMotionController = nullptr;
	APhysicsHandle = nullptr;
}

bool APickupStaticMeshActor::FIsHeldByMe_Implementation(USceneComponent * MotionController)
{
	return MotionController == AMotionController;
}

void APickupStaticMeshActor::Tick(float DeltaTime)
{
	if (EGrabMethod == EnumGrabMethod::AttachedTo) return;

	FDropIfLostConnection();

	if (EGrabMethod == EnumGrabMethod::PhysicalHandle) return;
	if (!IsValid(AMotionController)) return;

	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody),
		UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel2)
	};
	TArray<UPrimitiveComponent*> OutComponents;
	const bool IsOverlapping = UKismetSystemLibrary::ComponentOverlapComponents(
		GetStaticMeshComponent(),
		GetStaticMeshComponent()->GetComponentTransform(),
		ObjectTypes,
		nullptr,
		AActorsToIgnore,
		OutComponents
	);

	if (IsOverlapping) {
		if (IsGateAOpen) {
			IsGateAOpen = false;
			FDropAttachTo();
			FGrabPhysicsHandle();
			IsGateBOpen = true;
		}
	}
	else {
		const float GameTime = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());
		if ((GameTime - FHitTime) >= 0.1f) {
			if (FLocationAlpha > 1.0f) {
				if (IsGateBOpen) {
					IsGateBOpen = false;
					FDropPhysicsHandle();
					FGrabAttachTo();
					IsGateAOpen = true;
				}
			}
			else {
				FLocationAlpha = GameTime * 10.0f + FLocationAlpha;
				GetStaticMeshComponent()->SetWorldLocationAndRotation(
					FMath::VInterpTo(
						GetStaticMeshComponent()->GetComponentTransform().GetLocation(),
						FGetWorldPickupTransform().GetLocation(),
						GetWorld()->GetDeltaSeconds(),
						0.5f
					),
					FGetWorldPickupTransform().GetRotation()
				);
			}
		}
	}
}

void APickupStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

	// Ignored actors

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPMotionControllerClass, Actors);
	AActorsToIgnore = Actors;

	// Binding events

	this->GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &APickupStaticMeshActor::OnComponentHit);
}