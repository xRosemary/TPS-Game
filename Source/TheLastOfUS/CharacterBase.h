// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "CharacterBase.generated.h"

UCLASS()
class THELASTOFUS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere)
	UAnimInstance* AnimBP;

public:
	ACharacterBase();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	void EquipWeapon(AWeaponBase* weapon);

protected:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	AWeaponBase* OwnWeapon;

#pragma region KeyInput
protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void FirePressed();
	void FireReleased();
#pragma endregion


#pragma region Fire
protected:
	EWeaponType weaponType;
	void FireStart();
	void FireStop();
#pragma endregion


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	void StartWithKindofWeapon();
	void LoadWeapon(EWeaponType WeaponType);

	void AccelerateWalk();
	void NormalWalk();

	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

#pragma region NetWork
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAccelerateWalk();
	void ServerAccelerateWalk_Implementation();
	bool ServerAccelerateWalk_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNormalWalk();
	void ServerNormalWalk_Implementation();
	bool ServerNormalWalk_Validate();

	UFUNCTION(Server, Reliable)
	void ClientFire();
	void ClientFire_Implementation();
#pragma endregion
};
