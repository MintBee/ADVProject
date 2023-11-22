// Fill out your copyright notice in the Description page of Project Settings.


#include "ADVPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "PlayerAnim.h"
#include "Arrow.h"
// Sets default values
AADVPlayer::AADVPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("SkeletalMesh'/Game/Characters/Erika_Archer.Erika_Archer'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0, 0, 90));
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;

	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);


	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	JumpMaxCount = 1;

	unarmedBowMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UnarmedBowMeshComp"));
	unarmedBowMeshComp->SetupAttachment(GetMesh(), TEXT("Spine2Socket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempUnarmedBowMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Props/Bow.Bow'"));

	if (TempUnarmedBowMesh.Succeeded()) {
		unarmedBowMeshComp->SetSkeletalMesh(TempUnarmedBowMesh.Object);
		unarmedBowMeshComp->SetRelativeLocation(FVector(10.72, 0, -18.72));
		unarmedBowMeshComp->SetRelativeRotation(FRotator(-11.23, -183.68, 80.30));
	}

	armedBowMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmedBowMeshComp"));
	armedBowMeshComp->SetupAttachment(GetMesh(), TEXT("LeftHandSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempArmedBowMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Props/Bow.Bow'"));

	if (TempArmedBowMesh.Succeeded()) {
		armedBowMeshComp->SetSkeletalMesh(TempArmedBowMesh.Object);
		armedBowMeshComp->SetRelativeLocation(FVector(1.155857, -5.897994, 2.570458));
		armedBowMeshComp->SetRelativeRotation(FRotator(12.08, 97.60, 66.56));
		armedBowMeshComp->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AADVPlayer::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
}

// Called every frame
void AADVPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	direction = FTransform(GetControlRotation()).TransformVector(direction);
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void AADVPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("LookVertical"), this, &AADVPlayer::LookVertical);
	PlayerInputComponent->BindAxis(TEXT("LookHorizontal"), this, &AADVPlayer::LookHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &AADVPlayer::MoveVertical);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &AADVPlayer::MoveHorizontal);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AADVPlayer::InputJump);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AADVPlayer::InputRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &AADVPlayer::InputRun);
	PlayerInputComponent->BindAction(TEXT("Equip"), IE_Pressed, this, &AADVPlayer::InputEquip);
	PlayerInputComponent->BindAction(TEXT("AimandShoot"), IE_Pressed, this, &AADVPlayer::InputAim);
	PlayerInputComponent->BindAction(TEXT("AimandShoot"), IE_Released, this, &AADVPlayer::InputShoot);
	

}

void AADVPlayer::LookVertical(float value) {
	AddControllerPitchInput(value);
}
void AADVPlayer::LookHorizontal(float value) {
	AddControllerYawInput(value);
}
void AADVPlayer::MoveVertical(float value) {
	direction.X = value;
}
void AADVPlayer::MoveHorizontal(float value) {
	direction.Y = value;
}

void AADVPlayer::InputJump() {
	Jump();
}

void AADVPlayer::InputRun() {
	auto movement = GetCharacterMovement();

	if (movement->MaxWalkSpeed > walkSpeed) {
		movement->MaxWalkSpeed = walkSpeed;
	}
	else {
		movement->MaxWalkSpeed = runSpeed;
	}
}

void AADVPlayer::InputEquip() {
	auto movement = GetCharacterMovement();

	if (!bArmed) {
		bArmed = true;
		unarmedBowMeshComp->SetVisibility(false);
		armedBowMeshComp->SetVisibility(true);
	}
	else {
		bArmed = false;
		unarmedBowMeshComp->SetVisibility(true);
		armedBowMeshComp->SetVisibility(false);
	}

	if (movement->bOrientRotationToMovement)
		movement->bOrientRotationToMovement = false;
	else
		movement->bOrientRotationToMovement = true;

	if (bUseControllerRotationYaw)
		bUseControllerRotationYaw = false;
	else
		bUseControllerRotationYaw = true;
}

void AADVPlayer::InputAim() {
	if (!bArmed) {
		return;
	}

	bBowAim = true;
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	tpsCamComp->SetFieldOfView(45.0f);


}

void AADVPlayer::InputShoot() {
	if (!bArmed) {
		return;
	}
	FTransform shootPosition = armedBowMeshComp->GetSocketTransform(TEXT("ShootPosition"));
	GetWorld()->SpawnActor<AArrow>(arrowFactory, shootPosition);
	bBowAim = false;
	springArmComp->SetRelativeLocation(FVector(0, 0, 90));
	tpsCamComp->SetFieldOfView(90.0f);
}