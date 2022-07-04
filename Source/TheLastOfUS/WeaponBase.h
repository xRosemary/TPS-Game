// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "WeaponBase.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	AK47 UMETA(DisplayName = "AK47"),
	M4A1 UMETA(DisplayName = "M4A1"),
};

UCLASS()
class THELASTOFUS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponKind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnimMontage;

	UFUNCTION()
	void OnOtherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintImplementableEvent, Category="GunAnimation")
	void PlayFireAnimation();

	void StopFire();

	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void RemoveCollision();


public:	
	virtual void Tick(float DeltaTime) override;

};