// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "LoginMain.generated.h"

class UEditableText;
class UButton;

UCLASS()
class THELASTOFUS_API ULoginMain : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UEditableText* Account;

	UPROPERTY(meta = (BindWidget))
	UEditableText* Password;

	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RegisterButton;

public:
	ULoginMain(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct();
	virtual void NativeDestruct();

protected:
	UFUNCTION()
	void SignIn();

	UFUNCTION()
	void RegisterIn();

	void OnResponseReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

protected:
};
