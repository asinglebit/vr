// Fill out your copyright notice in the Description page of Project Settings.

#pragma once 

#include "CoreMinimal.h" 
#include "UObject/ObjectMacros.h" 

UENUM(BlueprintType)
enum EnumGrabMethod
{
	AttachedTo		UMETA(DisplayName = "AttachedTo"),
	PhysicalHandle	UMETA(DisplayName = "PhysicalHandle"),
	Mixed			UMETA(DisplayName = "Mixed")
};