#pragma once

#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"
#include "IJoystickPlugin.h"

const FKey EKeysJoystick::JoystickButton1("JoystickButton1");
const FKey EKeysJoystick::JoystickButton2("JoystickButton2");
const FKey EKeysJoystick::JoystickButton3("JoystickButton3");
const FKey EKeysJoystick::JoystickButton4("JoystickButton4");
const FKey EKeysJoystick::JoystickButton5("JoystickButton5");
const FKey EKeysJoystick::JoystickButton6("JoystickButton6");
const FKey EKeysJoystick::JoystickButton7("JoystickButton7");
const FKey EKeysJoystick::JoystickButton8("JoystickButton8");
const FKey EKeysJoystick::JoystickButton9("JoystickButton9");
const FKey EKeysJoystick::JoystickButton10("JoystickButton10");
const FKey EKeysJoystick::JoystickButton11("JoystickButton11");
const FKey EKeysJoystick::JoystickButton12("JoystickButton12");
const FKey EKeysJoystick::JoystickButton13("JoystickButton13");
const FKey EKeysJoystick::JoystickButton14("JoystickButton14");
const FKey EKeysJoystick::JoystickButton15("JoystickButton15");
const FKey EKeysJoystick::JoystickButton16("JoystickButton16");

const FKey EKeysJoystick::JoystickAxisX("JoystickAxisX");
const FKey EKeysJoystick::JoystickAxisY("JoystickAxisY");
const FKey EKeysJoystick::JoystickAxisZ("JoystickAxisZ");

const FKey EKeysJoystick::JoystickRAxisX("JoystickRAxisX");
const FKey EKeysJoystick::JoystickRAxisY("JoystickRAxisY");
const FKey EKeysJoystick::JoystickRAxisZ("JoystickRAxisZ");

const FKey EKeysJoystick::JoystickPOVX("JoystickPOVX");
const FKey EKeysJoystick::JoystickPOVY("JoystickPOVY");
const FKey EKeysJoystick::JoystickSlider("JoystickSlider");

/** Empty Event Functions, no Super call required, because they don't do anything! */
void JoystickDelegate::JoystickButtonPressed(int32 buttonNr, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonPressed(_interfaceDelegate, buttonNr, controller);
}

void JoystickDelegate::JoystickButtonReleased(int32 buttonNr, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonReleased(_interfaceDelegate, buttonNr, controller);
}

void JoystickDelegate::AxisChanged(FVector AxisValue, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickAxisChanged(_interfaceDelegate, AxisValue, controller);
}

void JoystickDelegate::RAxisChanged(FVector AxisValue, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickRAxisChanged(_interfaceDelegate, AxisValue, controller);
}

void JoystickDelegate::POVChanged(JoystickPOVDirection POVValue, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickPOVChanged(_interfaceDelegate, POVValue, controller);
}

void JoystickDelegate::SliderChanged(float SliderValue, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickSliderChanged(_interfaceDelegate, SliderValue, controller);
}

bool JoystickDelegate::implementsInterface()
{
	return (_interfaceDelegate != NULL && _interfaceDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()));
}

//Setting
void JoystickDelegate::SetInterfaceDelegate(UObject* newDelegate)
{
	UE_LOG(LogClass, Log, TEXT("InterfaceDelegate passed: %s"), *newDelegate->GetName());

	//Use this format to support both blueprint and C++ form
	if (newDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
	{
		_interfaceDelegate = newDelegate;
	}
	else
	{
		//Try casting as self
		if (ValidSelfPointer->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
		{
			_interfaceDelegate = (UObject*)this;
		}
		else
		{
			//If you're crashing its probably because of this setting causing an assert failure
			_interfaceDelegate = NULL;
			UE_LOG(LogClass, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface delegate is NULL, did your class implement JoystickInterface? Events are disabled."));
		}

		//Either way post a warning, this will be a common error
		UE_LOG(LogClass, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface Delegate is NOT set, did your class implement JoystickInterface? Events are disabled."));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("JoystickBlueprintDelegate Warning: Delegate is NOT set, did your class implement JoystickInterface? Events are disabled."));
	}
}

//Startup
void JoystickDelegate::JoystickStartup()
{
	if (IJoystickPlugin::IsAvailable())
	{
		//Required to Work - Set self as a delegate
		IJoystickPlugin::Get().SetDelegate((JoystickDelegate*)this);
	}

	UObject* validUObject = NULL;
	validUObject = Cast<UObject>(ValidSelfPointer);

	//Set self as interface delegate by default
	if (!_interfaceDelegate && validUObject)
		SetInterfaceDelegate(validUObject);
}

UJoystickSingleController* JoystickDelegate::JoystickGetLatestFrame()
{
	return _latestFrame;
}

bool JoystickDelegate::JoystickIsAvailable()
{
	return true;
}

void JoystickDelegate::JoystickTick(float DeltaTime)
{
	if (IJoystickPlugin::IsAvailable())
	{
		//Required to Work - This is the plugin magic
		IJoystickPlugin::Get().JoystickTick(DeltaTime);
	}
}
