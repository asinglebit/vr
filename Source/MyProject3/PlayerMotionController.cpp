#include "PlayerMotionController.h"

#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "HandsMotionController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "InputCoreTypes.h"

// Sets default values
APlayerMotionController::APlayerMotionController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerMotionController::BeginPlay()
{
	Super::BeginPlay();
	//UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

	// Spawn left hand
	/*{
		UObject* ClassPackage = ANY_PACKAGE;
		UObject* ObjectToSpawn;
		ObjectToSpawn = FindObject<UObject>(ClassPackage, TEXT("/Game/Blueprints/BP_MotionController.BP_MotionController"));
		UClass* ClassToSpawn = ObjectToSpawn->StaticClass();
		AActor * NewActor = GetWorld()->SpawnActorDeferred<AActor>(ClassToSpawn, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		//NewActor->EHand = EControllerHand::Left;
		UGameplayStatics::FinishSpawningActor(NewActor, FTransform::Identity);
		auto components = this->GetComponent();
		USceneComponent* SceneComponent = nullptr;
		for (auto component : components)
		{
			if (component->GetFName() == "VROrigin")
			{
				SceneComponent = Cast<USceneComponent>(component);
			}
		}
		NewActor->AttachToComponent(SceneComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false));
		//NewActor->AttachToComponent(, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false));
	}*/

	// Spawn right hand

	/*{
		UObject* ClassPackage = ANY_PACKAGE;
		UObject* ObjectToSpawn;
		ObjectToSpawn = FindObject<UObject>(ClassPackage, TEXT("/Game/Blueprints/BP_MotionController.BP_MotionController"));
		UClass* ClassToSpawn = ObjectToSpawn->StaticClass();
		AHandsMotionController * NewActor = GetWorld()->SpawnActorDeferred<AHandsMotionController>(ClassToSpawn, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		//NewActor->EHand = EControllerHand::Right;
		UGameplayStatics::FinishSpawningActor(NewActor, FTransform::Identity);
		//auto components = this->GetComponents();
		//NewActor->AttachToComponent(, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false));
	}*/
}

// Called every frame
void APlayerMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerMotionController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
