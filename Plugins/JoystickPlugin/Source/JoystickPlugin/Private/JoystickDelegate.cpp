#pragma once

#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"
#include "IJoystickPlugin.h"

const FKey EKeysJoystick::JoystickButton[32] = {
	FKey("JoystickButton1"),
	FKey("JoystickButton2"),
	FKey("JoystickButton3"),
	FKey("JoystickButton4"),
	FKey("JoystickButton5"),
	FKey("JoystickButton6"),
	FKey("JoystickButton7"),
	FKey("JoystickButton8"),
	FKey("JoystickButton9"),
	FKey("JoystickButton10"),
	FKey("JoystickButton11"),
	FKey("JoystickButton12"),
	FKey("JoystickButton13"),
	FKey("JoystickButton14"),
	FKey("JoystickButton15"),
	FKey("JoystickButton16"),
	FKey("JoystickButton17"),
	FKey("JoystickButton18"),
	FKey("JoystickButton19"),
	FKey("JoystickButton20"),
	FKey("JoystickButton21"),
	FKey("JoystickButton22"),
	FKey("JoystickButton23"),
	FKey("JoystickButton24"),
	FKey("JoystickButton25"),
	FKey("JoystickButton26"),
	FKey("JoystickButton27"),
	FKey("JoystickButton28"),
	FKey("JoystickButton29"),
	FKey("JoystickButton30"),
	FKey("JoystickButton31"),
	FKey("JoystickButton32")
};

const FKey EKeysJoystick::JoystickAxisX("JoystickAxisX");
const FKey EKeysJoystick::JoystickAxisY("JoystickAxisY");
const FKey EKeysJoystick::JoystickAxisZ("JoystickAxisZ");

const FKey EKeysJoystick::JoystickRAxisX("JoystickRAxisX");
const FKey EKeysJoystick::JoystickRAxisY("JoystickRAxisY");
const FKey EKeysJoystick::JoystickRAxisZ("JoystickRAxisZ");

const FKey EKeysJoystick::JoystickPOVX[] {
	FKey("JoystickPOVX1"),
		FKey("JoystickPOVX2"),
		FKey("JoystickPOVX3"),
		FKey("JoystickPOVX4")
};

const FKey EKeysJoystick::JoystickPOVY[] {
	FKey("JoystickPOVY1"),
		FKey("JoystickPOVY2"),
		FKey("JoystickPOVY3"),
		FKey("JoystickPOVY4")
};

const FKey EKeysJoystick::JoystickSlider[] {
	FKey("JoystickSlider1"),
		FKey("JoystickSlider2")
};

FVector2D POVAxis(JoystickPOVDirection povValue)
{
	switch (povValue){
	case DIRECTION_NONE:
		return FVector2D(0, 0);
	case DIRECTION_UP:
		return FVector2D(0, 1);
	case DIRECTION_UP_RIGHT:
		return FVector2D(1, 1);
	case DIRECTION_RIGHT:
		return FVector2D(1, 0);
	case DIRECTION_DOWN_RIGHT:
		return FVector2D(1, -1);
	case DIRECTION_DOWN:
		return FVector2D(0, -1);
	case DIRECTION_DOWN_LEFT:
		return FVector2D(-1, -1);
	case DIRECTION_LEFT:
		return FVector2D(-1, 0);
	case DIRECTION_UP_LEFT:
		return FVector2D(-1, 1);
	default:
		return FVector2D(0, 0);
	}
}

/** Empty Event Functions, no Super call required, because they don't do anything! */
void JoystickDelegate::JoystickButtonPressed(int32 buttonNr, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonPressed(_interfaceDelegate, buttonNr, player);
}

void JoystickDelegate::JoystickButtonReleased(int32 buttonNr, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonReleased(_interfaceDelegate, buttonNr, player);
}

void JoystickDelegate::AxisChanged(FVector AxisValue, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickAxisChanged(_interfaceDelegate, AxisValue, player);
}

void JoystickDelegate::RAxisChanged(FVector AxisValue, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickRAxisChanged(_interfaceDelegate, AxisValue, player);
}

void JoystickDelegate::POVChanged(JoystickPOVDirection POVValue, int32 index, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickPOVChanged(_interfaceDelegate, POVValue, index, player);
}

void JoystickDelegate::SliderChanged(FVector2D SliderValue, int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickSliderChanged(_interfaceDelegate, SliderValue, player);
}

void JoystickDelegate::JoystickPluggedIn(int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickPluggedIn(_interfaceDelegate, player);
}
void JoystickDelegate::JoystickUnplugged(int32 player)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickUnplugged(_interfaceDelegate, player);
}

bool JoystickDelegate::implementsInterface()
{
	return (_interfaceDelegate != nullptr && _interfaceDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()));
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
		IJoystickPlugin::Get().SetDelegate(this);
	}

	UObject* validUObject = Cast<UObject>(ValidSelfPointer);

	//Set self as interface delegate by default
	if (!_interfaceDelegate && validUObject)
		SetInterfaceDelegate(validUObject);
}

void JoystickDelegate::JoystickStop()
{
	if (IJoystickPlugin::IsAvailable())
	{
		//Required to Work - Set self as a delegate
		IJoystickPlugin::Get().SetDelegate(nullptr);
	}
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
