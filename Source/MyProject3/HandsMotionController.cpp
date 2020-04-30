// Fill out your copyright notice in the Description page of Project Settings.

#include "HandsMotionController.h"

#include "EngineUtils.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineMeshComponent.h"
#include "MotionControllerComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/PickupActor.h"
#include "NavigationSystem.h"

// Sets default values
AHandsMotionController::AHandsMotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SMesh(TEXT("/Game/Meshes/BeamMesh.BeamMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SMaterial(TEXT("/Game/Materials/M_SplineArcMat.M_SplineArcMat"));

	this->Mesh = SMesh.Object;
	this->Material = SMaterial.Object;
}

void AHandsMotionController::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Get components

	auto components = this->GetComponents();
	for (auto component : components)
	{
		const FName ComponentName = component->GetFName();
		if (ComponentName == "TeleportCylinder") {
			this->TeleportCylinder = CastChecked<USceneComponent>(component);
		}
		else if (ComponentName == "GrabSphere") {
			this->GrabSphere = CastChecked<USphereComponent>(component);
		}
		else if (ComponentName == "ArcSpline") {
			this->ArcSpline = CastChecked<USplineComponent>(component);
		}
		else if (ComponentName == "PhysicsHandle") {
			this->APhysicsHandle = CastChecked<UPhysicsHandleComponent>(component);
		}
		else if (ComponentName == "MotionController") {
			this->AMotionController = CastChecked<UMotionControllerComponent>(component);
		}
		else if (ComponentName == "ArcEndPoint") {
			this->ArcEndPoint = CastChecked<USceneComponent>(component);
		}
	}

	FString HandString = UEnum::GetValueAsString(this->EHand.GetValue());
	this->AMotionController->SetTrackingMotionSource(FName(*HandString));
}

// Called when the game starts or when spawned
void AHandsMotionController::BeginPlay()
{
	Super::BeginPlay();

	// Initialize variables

	this->FTeleportLaunchVelocity = 900.0f;
	this->BIsTeleporterActive = false;
	this->BIsValidTeleportDestination = false;
	this->BIsToBeGripped = false;

	// Set construction logic

	this->TeleportCylinder->SetVisibility(true, true);
}

void AHandsMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->FClearArc();
	if (this->BIsTeleporterActive) {

		// Call
		TArray<FVector> TracePoints;
		FVector NavMeshLocation;
		FVector TraceLocation;
		const bool IsHit = this->FTraceTeleportDestination(TracePoints, NavMeshLocation, TraceLocation);

		this->BIsValidTeleportDestination = IsHit;
		this->TeleportCylinder->SetVisibility(IsHit, true);

		// Call
		UObject* WorldContextObject = GetWorld();
		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic) };
		const TArray<AActor*> ActorsToIgnore;
		FHitResult OutHit;
		UKismetSystemLibrary::LineTraceSingleForObjects(
			WorldContextObject,
			NavMeshLocation,
			NavMeshLocation + FVector(0.0f, 0.0f, -200.0f),
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			OutHit,
			true,
			FLinearColor(1.0f, 0.0f, 0.0f),
			FLinearColor(0.0f, 1.0f, 0.0f),
			5.0f
		);

		this->TeleportCylinder->SetWorldLocation(
			UKismetMathLibrary::SelectVector(
				OutHit.ImpactPoint,
				NavMeshLocation,
				OutHit.bBlockingHit
			),
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);

		this->BLastFrameValidDestination = IsHit;
		this->FUpdateArcSpline(IsHit, TracePoints);
		this->FUpdateArcEndpoint(IsHit, TraceLocation);
	}

	const FTransform Transform = this->AMotionController->GetComponentTransform();
	this->APhysicsHandle->SetTargetLocationAndRotation(
		Transform.GetLocation(),
		Transform.GetRotation().Rotator()
	);
}

void AHandsMotionController::FClearArc()
{
	this->ArcSpline->ClearSplinePoints(true);
	for (USplineMeshComponent* component : this->ASplineMeshes) {
		component->DestroyComponent();
	}
	this->ASplineMeshes.Empty();
}

void AHandsMotionController::FActivateTeleporter()
{
	this->BIsTeleporterActive = true;
	this->TeleportCylinder->SetVisibility(true, true);
}

void AHandsMotionController::FDisableTeleporter()
{
	if (this->BIsTeleporterActive) {
		this->BIsTeleporterActive = false;
		this->TeleportCylinder->SetVisibility(false, true);
		this->ArcEndPoint->SetVisibility(false, true);
	}
}

FVector AHandsMotionController::FGetTeleportDestination()
{
	return this->TeleportCylinder->GetComponentLocation();
}

void AHandsMotionController::FUpdateArcEndpoint(bool IsValidLocationFound, FVector NewLocation)
{
	const bool isVisible = IsValidLocationFound && this->BIsTeleporterActive;
	this->ArcEndPoint->SetVisibility(isVisible, true);
	this->ArcEndPoint->SetWorldLocation(NewLocation);
}

void AHandsMotionController::FUpdateArcSpline(bool IsValidLocationFound, TArray<FVector> SplinePoints)
{
	if (!IsValidLocationFound) {
		SplinePoints.Empty();

		const FVector MotionControllerLocation = this->AMotionController->GetComponentLocation();
		const FVector MotionControllerForwardVector = this->AMotionController->GetForwardVector();

		SplinePoints.Add(MotionControllerLocation);
		SplinePoints.Add(MotionControllerForwardVector * 20.0f + MotionControllerLocation);
	}

	for (auto SplinePoint : SplinePoints) {
		this->ArcSpline->AddSplinePoint(SplinePoint, ESplineCoordinateSpace::Local, true);
	}

	this->ArcSpline->SetSplinePointType(SplinePoints.IndexOfByKey(SplinePoints.Last()), ESplinePointType::CurveClamped, true);

	const int lastIndex = this->ArcSpline->GetNumberOfSplinePoints() - 1;
	for (int32 i = 0; i < lastIndex; ++i) {
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->RegisterComponent();
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->bCastDynamicShadow = false;
		SplineMesh->SetStartScale(FVector2D(4.0f, 4.0f));
		SplineMesh->SetEndScale(FVector2D(4.0f, 4.0f));
		SplineMesh->SetStaticMesh(this->Mesh);
		SplineMesh->SetMaterial(0, this->Material);
		const FVector StartPosition = SplinePoints[i];
		const FVector StartTangent = this->ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPosition = SplinePoints[i + 1];
		const FVector EndTangent = this->ArcSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		SplineMesh->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent, true);
		this->ASplineMeshes.Add(SplineMesh);
	}
}

bool AHandsMotionController::FTraceTeleportDestination(TArray<FVector> & TracePoints, FVector & NavMeshLocation, FVector & TraceLocation)
{
	// Local variables

	const float ProjectNavExtends = 500.0f;

	// Input

	UObject* WorldContextObject = GetWorld();
	const FVector StartPosition = this->AMotionController->GetComponentLocation();
	const FVector LaunchVelocity = this->AMotionController->GetForwardVector() * this->FTeleportLaunchVelocity;
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic) };
	const TArray<AActor*> ActorsToIgnore;

	// Call

	FHitResult OutHit;
	TArray<FVector> OutPathPositions;
	FVector OutLastTraceDestination;
	const bool IsHit = UGameplayStatics::Blueprint_PredictProjectilePath_ByObjectType(
		WorldContextObject,
		OutHit,
		OutPathPositions,
		OutLastTraceDestination,
		StartPosition,
		LaunchVelocity,
		true,
		0.0f,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		0.0f,
		30.0f,
		2.0f,
		0.0f
	);

	// Call

	FNavLocation ProjectedLocation;
	UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	bool IsHitNav = NavigationSystem->ProjectPointToNavigation(
		OutHit.Location,
		ProjectedLocation,
		FVector(ProjectNavExtends, ProjectNavExtends, ProjectNavExtends)
	);

	// Output
	TracePoints = OutPathPositions;
	NavMeshLocation = ProjectedLocation;
	TraceLocation = OutHit.Location;
	return IsHit && IsHitNav;
}

AActor* AHandsMotionController::FGetActorNearHand()
{
	float NearestOverlap = 10000.0f;
	AActor* NearestOverlappingActor = nullptr;
	TArray<AActor*> OverlappingActors;
	this->GrabSphere->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors) {
		const bool IsPickupActor = UKismetSystemLibrary::DoesImplementInterface(OverlappingActor, UPickupActor::StaticClass());
		if (IsPickupActor) {
			const FVector DifferenceVector = OverlappingActor->GetActorLocation() - this->GrabSphere->GetComponentLocation();
			const float VectorLength = DifferenceVector.Size();
			if (VectorLength < NearestOverlap) {
				NearestOverlappingActor = OverlappingActor;
				NearestOverlap = VectorLength;
			}
		}
	}

	return NearestOverlappingActor;
}

void AHandsMotionController::FGrabActor()
{
	const bool a = true;
	this->BIsToBeGripped = true;
	AActor* ActorToBeGripped = this->FGetActorNearHand();
	if (ActorToBeGripped != NULL) {
		this->AttachedActor = ActorToBeGripped;		
		IPickupActor::Execute_FPickup(ActorToBeGripped, this->AMotionController, this->APhysicsHandle);
	}
}

void AHandsMotionController::FReleaseActor()
{
	this->BIsToBeGripped = false;
	if (this->AttachedActor != NULL) {
		const bool IsHeldByMe = IPickupActor::Execute_FIsHeldByMe(this->AttachedActor, this->AMotionController);
		if (IsHeldByMe) {
			IPickupActor::Execute_FDrop(this->AttachedActor);
		}
		this->AttachedActor = nullptr;
	}
}