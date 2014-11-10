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
const FKey EKeysJoystick::JoystickButton17("JoystickButton17");
const FKey EKeysJoystick::JoystickButton18("JoystickButton18");
const FKey EKeysJoystick::JoystickButton19("JoystickButton19");
const FKey EKeysJoystick::JoystickButton20("JoystickButton20");
const FKey EKeysJoystick::JoystickButton21("JoystickButton21");
const FKey EKeysJoystick::JoystickButton22("JoystickButton22");
const FKey EKeysJoystick::JoystickButton23("JoystickButton23");
const FKey EKeysJoystick::JoystickButton24("JoystickButton24");
const FKey EKeysJoystick::JoystickButton25("JoystickButton25");
const FKey EKeysJoystick::JoystickButton26("JoystickButton26");
const FKey EKeysJoystick::JoystickButton27("JoystickButton27");
const FKey EKeysJoystick::JoystickButton28("JoystickButton28");
const FKey EKeysJoystick::JoystickButton29("JoystickButton29");
const FKey EKeysJoystick::JoystickButton30("JoystickButton30");
const FKey EKeysJoystick::JoystickButton31("JoystickButton31");
const FKey EKeysJoystick::JoystickButton32("JoystickButton32");

const FKey EKeysJoystick::JoystickAxisX("JoystickAxisX");
const FKey EKeysJoystick::JoystickAxisY("JoystickAxisY");
const FKey EKeysJoystick::JoystickAxisZ("JoystickAxisZ");

const FKey EKeysJoystick::JoystickRAxisX("JoystickRAxisX");
const FKey EKeysJoystick::JoystickRAxisY("JoystickRAxisY");
const FKey EKeysJoystick::JoystickRAxisZ("JoystickRAxisZ");

const FKey EKeysJoystick::JoystickPOV1X("JoystickPOVX1");
const FKey EKeysJoystick::JoystickPOV1Y("JoystickPOVY1");

const FKey EKeysJoystick::JoystickPOV2X("JoystickPOVX2");
const FKey EKeysJoystick::JoystickPOV2Y("JoystickPOVY2");

const FKey EKeysJoystick::JoystickPOV3X("JoystickPOVX3");
const FKey EKeysJoystick::JoystickPOV3Y("JoystickPOVY3");

const FKey EKeysJoystick::JoystickSlider1("JoystickSlider1");
const FKey EKeysJoystick::JoystickSlider2("JoystickSlider2");

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

void JoystickDelegate::SliderChanged(FVector2D SliderValue, UJoystickSingleController* controller)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickSliderChanged(_interfaceDelegate, SliderValue, controller);
}

void JoystickDelegate::JoystickPluggedIn()
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickPluggedIn(_interfaceDelegate);
}
void JoystickDelegate::JoystickUnplugged()
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickUnplugged(_interfaceDelegate);
}

bool JoystickDelegate::implementsInterface()
{
	return (_interfaceDelegate != NULL && _interfaceDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()));
}

//Setting
void JoystickDelegate::SetInterfaceDelegate(UObject* newDelegate)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("InterfaceDelegate passed: %s"), *newDelegate->GetName());

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
			UE_LOG(JoystickPluginLog, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface delegate is NULL, did your class implement JoystickInterface? Events are disabled."));
		}

		//Either way post a warning, this will be a common error
		UE_LOG(JoystickPluginLog, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface Delegate is NOT set, did your class implement JoystickInterface? Events are disabled."));
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
