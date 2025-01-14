// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lever.generated.h"

UCLASS()
class ADVPROJECT_API ALever : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class UBoxComponent* colliComp;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	bool isTriggered = false;

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
