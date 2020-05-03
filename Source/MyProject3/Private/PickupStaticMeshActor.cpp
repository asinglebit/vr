#include "../Public/PickupStaticMeshActor.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

APickupStaticMeshActor::APickupStaticMeshActor()
{
	static ConstructorHelpers::FClassFinder<AHandsMotionController> ActorClassFinder(TEXT("/Game/Blueprints/BP_MotionController"));
	BPMotionControllerClass = ActorClassFinder.Class;
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