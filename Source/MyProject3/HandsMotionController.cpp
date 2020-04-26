// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "HandsMotionController.h"

// Sets default values
AHandsMotionController::AHandsMotionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHandsMotionController::BeginPlay()
{
	Super::BeginPlay();

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
	}

	// Set construction logic

	this->TeleportCylinder->SetVisibility(true, true);
	//this->SplineMeshes = [];

	//for (int32 i = 0; i < mSplineComponent->GetNumSplinePoints() - 1; i++)
	//{
	//	USplineMeshComponent* SplineMesh = ConstructObject<USplineMeshComponent>(USplineMeshComponent::StaticClass(), this);

	//	SplineMesh->bCreatedByConstructionScript = true;
	//	SplineMesh->SetMobility(EComponentMobility::Movable);
	//	SplineMesh->AttachParent = mSplineComponent;

	//	//Set the color!
	//	UMaterialInstanceDynamic* dynamicMat = UMaterialInstanceDynamic::Create(mSplineMeshMaterial, NULL);
	//	dynamicMat->SetVectorParameterValue(TEXT("Color"), FLinearColor(mSegments[i].mColor));

	//	SplineMesh->bCastDynamicShadow = false;
	//	SplineMesh->SetStaticMesh(mGridMesh);
	//	SplineMesh->SetMaterial(0, dynamicMat);

	//	//Width of the mesh 
	//	SplineMesh->SetStartScale(FVector2D(50, 50));
	//	SplineMesh->SetEndScale(FVector2D(50, 50));

	//	FVector pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd;
	//	mSplineComponent->GetLocalLocationAndTangentAtSplinePoint(i, pointLocationStart, pointTangentStart);
	//	mSplineComponent->GetLocalLocationAndTangentAtSplinePoint(i + 1, pointLocationEnd, pointTangentEnd);

	//	SplineMesh->SetStartAndEnd(pointLocationStart, pointTangentStart, pointLocationEnd, pointTangentEnd);
	//}

	//RegisterAllComponents();
	
	//this->ArcSpline->GetNumberOfSplinePoints();

}

// Called every frame
void AHandsMotionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called every frame
void AHandsMotionController::FClearArc()
{
	this->ArcSpline->ClearSplinePoints(true);
	for (USplineMeshComponent* component : this->ASplineMeshes) {
		component->DestroyComponent();
	}
	this->ASplineMeshes.Empty();
}
