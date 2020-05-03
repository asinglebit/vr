#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "../Public/PickupStaticMeshActor.h"

APickupStaticMeshActor::APickupStaticMeshActor()
{
	static ConstructorHelpers::FClassFinder<AHandsMotionController> ActorClassFinder(TEXT("/Game/Blueprints/BP_MotionController"));
	BPMotionControllerClass = ActorClassFinder.Class;
}

void APickupStaticMeshActor::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void APickupStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), BPMotionControllerClass, Actors);
	AActorsToIgnore = Actors;
}