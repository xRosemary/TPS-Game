// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	this->StartWithKindofWeapon();
	AnimBP = Mesh->GetAnimInstance();
}


void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Accelerate", IE_Pressed,this, &ACharacterBase::AccelerateWalk);
	PlayerInputComponent->BindAction("Accelerate", IE_Released,this, &ACharacterBase::NormalWalk);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::FirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::FireReleased);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
}


#pragma region Fire
void ACharacterBase::FirePressed()
{
	switch (weaponType)
	{
	case EWeaponType::AK47:
	{
		FireStart();
		break;
	}
	case EWeaponType::M4A1:
	{
		FireStart();
		break;
	}
	default:
		break;
	}
}

void ACharacterBase::FireReleased()
{
	switch (weaponType)
	{
	case EWeaponType::AK47:
	{
		FireStop();
		break;
	}
	case EWeaponType::M4A1:
	{
		FireStop();
		break;
	}
	default:
		break;
	}
}

void ACharacterBase::FireStart()
{
	ClientFire();
}

void ACharacterBase::FireStop()
{
}

#pragma endregion


#pragma region Weapon
void ACharacterBase::EquipWeapon(AWeaponBase* weapon)
{
	if (this->OwnWeapon) {

	}
	else
	{
		this->OwnWeapon = weapon;
		this->OwnWeapon->SetOwner(this);
		this->OwnWeapon->K2_AttachToComponent(Mesh, TEXT("WeaponSocket"),
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			true);
	}

}

void ACharacterBase::LoadWeapon(EWeaponType WeaponType)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	switch (WeaponType)
	{
	case EWeaponType::AK47:
	{
		UClass* BlueprintVar = StaticLoadClass(AWeaponBase::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/AK47.AK47_C'"));
		AWeaponBase* weapon = GetWorld()->SpawnActor<AWeaponBase>(BlueprintVar, GetActorTransform(), SpawnInfo);
		EquipWeapon(weapon);
		break;
	}
	case EWeaponType::M4A1:
	{
		UClass* BlueprintVar = StaticLoadClass(AWeaponBase::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprint/Weapon/M4A1.M4A1_C'"));
		AWeaponBase* weapon = GetWorld()->SpawnActor<AWeaponBase>(BlueprintVar, GetActorTransform(), SpawnInfo);
		EquipWeapon(weapon);
		break;
	}
	default:
		break;
	}
}

void ACharacterBase::StartWithKindofWeapon()
{
	if (HasAuthority())
	{
		LoadWeapon(EWeaponType::M4A1);
		this->weaponType = EWeaponType::M4A1;
	}
}
#pragma endregion

#pragma region MovementRPC
void ACharacterBase::ServerAccelerateWalk_Implementation()
{
	CharacterMovement->MaxWalkSpeed = 600;
}

bool ACharacterBase::ServerAccelerateWalk_Validate()
{
	return true;
}

void ACharacterBase::ServerNormalWalk_Implementation()
{
	CharacterMovement->MaxWalkSpeed = 400;
}

bool ACharacterBase::ServerNormalWalk_Validate()
{
	return true;
}

void ACharacterBase::ClientFire_Implementation()
{
	if (OwnWeapon)
	{
		OwnWeapon->PlayFireAnimation();
	}
	UAnimMontage* FireMontage = OwnWeapon->FireAnimMontage;
	if (FireMontage) {
		UE_LOG(LogTemp, Warning, TEXT("123"));
	}
	AnimBP->Montage_SetPlayRate(FireMontage, 1);
	AnimBP->Montage_Play(FireMontage);
}

void ACharacterBase::AccelerateWalk()
{
	CharacterMovement->MaxWalkSpeed = 600;
	ServerAccelerateWalk();
}

void ACharacterBase::NormalWalk()
{
	CharacterMovement->MaxWalkSpeed = 400;
	ServerNormalWalk();
}


void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACharacterBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

#pragma endregion