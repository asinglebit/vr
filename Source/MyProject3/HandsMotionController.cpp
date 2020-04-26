// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "MotionControllerComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "UObject/UObjectGlobals.h"

#include "HandsMotionController.h"

// Sets default values
AHandsMotionController::AHandsMotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SMesh(TEXT("/Game/Meshes/BeamMesh.BeamMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SMaterial(TEXT("/Game/Materials/M_SplineArcMat.M_SplineArcMat"));

	this->Mesh = SMesh.Object;
	this->Material = SMaterial.Object;
}

// Called when the game starts or when spawned
void AHandsMotionController::BeginPlay()
{
	Super::BeginPlay();

	// Initialize variables

	this->FTeleportLaunchVelocity = 900.0f;
	this->BIsTeleporterActive = false;
	this->BIsValidTeleportDestination = false;

	// Get components

	auto components = this->GetComponents();
	for (auto component : components)
	{
		const FName ComponentName = component->GetFName();
		if (ComponentName == "TeleportCylinder") {
			this->TeleportCylinder = CastChecked<USceneComponent>(component);
		}
		else if (ComponentName == "ArcSpline") {
			this->ArcSpline = CastChecked<USplineComponent>(component);
		}
		else if (ComponentName == "MotionController") {
			this->MotionController = CastChecked<UMotionControllerComponent>(component);
		}
		else if (ComponentName == "ArcEndPoint") {
			this->ArcEndPoint = CastChecked<USceneComponent>(component);
		}
	}

	// Set construction logic

	this->TeleportCylinder->SetVisibility(true, true);
}

void AHandsMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	this->ArcEndPoint->SetVisibility(isVisible, false);
	this->ArcEndPoint->SetWorldLocation(NewLocation);
}

void AHandsMotionController::FUpdateArcSpline(bool IsValidLocationFound, TArray<FVector> SplinePoints)
{
	if (!IsValidLocationFound) {
		SplinePoints.Empty();

		const FVector MotionControllerLocation = this->MotionController->GetComponentLocation();
		const FVector MotionControllerForwardVector = this->MotionController->GetForwardVector();

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