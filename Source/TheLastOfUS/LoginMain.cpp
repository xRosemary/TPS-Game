// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginMain.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "HttpModule.h"
#include "Serialization/JsonSerializerMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

ULoginMain::ULoginMain(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void ULoginMain::NativeConstruct()
{
	Super::NativeConstruct();
	LoginButton->OnPressed.AddDynamic(this, &ULoginMain::SignIn);
	RegisterButton->OnPressed.AddDynamic(this, &ULoginMain::RegisterIn);
}

void ULoginMain::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULoginMain::OnResponseReceived(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		// create a json object
		TSharedPtr<FJsonObject> ResponseObj;
		
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());

		// Deserialize
		FJsonSerializer::Deserialize(Reader, ResponseObj);
		// UKismetSystemLibrary::PrintString(this, HttpResponse->GetContentAsString());
		FString response = ResponseObj->GetStringField("msg");
		if (response.Equals("AllowLogin")) 
		{
			FString LobbyClassLoadPath = FString(TEXT("WidgetBlueprint'/Game/Blueprint/UI/Lobby/UI_ConnectMenu_BP.UI_ConnectMenu_BP_C'"));
			UClass* Widget = LoadClass<UUserWidget>(NULL, *LobbyClassLoadPath);
			UUserWidget* LobbyUI = CreateWidget<UUserWidget>(GetWorld(), Widget);
			LobbyUI->AddToViewport();
			RemoveFromParent();
		}
		else if (response.Equals("AllowRegister")) 
		{
		}
	}
}

void ULoginMain::RegisterIn()
{
}

void ULoginMain::SignIn()
{
	FString AccountString = Account->GetText().ToString();
	FString PasswordString = Password->GetText().ToString();

	// Create a http request
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	// Create a json object
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("Account", AccountString);
	RequestObj->SetStringField("Password", PasswordString);

	// Define a content object
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	
	FJsonSerializer::Serialize(RequestObj, Writer);

	Request->OnProcessRequestComplete().BindUObject(this, &ULoginMain::OnResponseReceived);
	Request->SetURL("http://127.0.0.1:8000/api/login/");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(RequestBody);
	Request->ProcessRequest();
}