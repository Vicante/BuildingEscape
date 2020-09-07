// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/World.h"
#include <GameFramework/Actor.h>
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	CloseAngle = GetOwner()->GetActorRotation().Yaw;
	OpenAngle += CloseAngle;
	CurrentAngle = CloseAngle;

	if (!PressurePlate) 
	{
		FString ActorName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("%s has a OpenDoor component, but does not have an assigned Pressure Plate."), *ActorName);
	}
	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassThatOpens)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();	
	}
	else 
	{
		float CurrentDeltaTime = FMath::Abs(DoorLastOpened - GetWorld()->GetTimeSeconds());
		if (CurrentDeltaTime > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}	
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	CurrentAngle = FMath::FInterpTo(CurrentAngle, OpenAngle, DeltaTime, DoorOpenSpeed);
	FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentAngle, CurrentRotation.Roll);
	GetOwner()->SetActorRotation(NewRotation);

	if (!AudioComponent) { return; }

	if (!BIsDoorOpened)
	{
		AudioComponent->Play();
		BIsDoorOpened = true;
	}
	
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	CurrentAngle = FMath::FInterpTo(CurrentAngle, CloseAngle, DeltaTime, DoorCloseSpeed);
	FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentAngle, CurrentRotation.Roll);
	GetOwner()->SetActorRotation(NewRotation);

	if (!AudioComponent) { return; }

	if (BIsDoorOpened)
	{
		AudioComponent->Play();
		BIsDoorOpened = false;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.0f;
	TArray<AActor*> OverlappingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += (Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass());	
	}
	return TotalMass;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!AudioComponent) 
	{
		UE_LOG(LogTemp, Error, TEXT("%s does not have a Audio Component!"), *GetOwner()->GetName());
	}
}
