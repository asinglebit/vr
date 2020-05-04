#include "../Public/PickupStaticMeshActor.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

APickupStaticMeshActor::APickupStaticMeshActor()
{
	static ConstructorHelpers::FClassFinder<AHandsMotionController> ActorClassFinder(TEXT("/Game/Blueprints/BP_MotionController"));
	BPMotionControllerClass = ActorClassFinder.Class;

	EGrabMethod = EnumGrabMethod::Mixed;
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
	if (!AMotionController->IsValidLowLevel()) return;

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