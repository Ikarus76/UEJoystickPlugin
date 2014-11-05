#pragma once

#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "IJoystickPlugin.h"

/** Empty Event Functions, no Super call required, because they don't do anything! */
void JoystickDelegate::JoystickButtonPressed(int32 ButtonNr){}
void JoystickDelegate::JoystickButtonReleased(int32 ButtonNr){}
void JoystickDelegate::XAxisChanged(int32 Value){}
void JoystickDelegate::YAxisChanged(int32 Value){}
void JoystickDelegate::ZAxisChanged(int32 Value){}
void JoystickDelegate::RXAxisChanged(int32 Value){}
void JoystickDelegate::RYAxisChanged(int32 Value){}
void JoystickDelegate::RZAxisChanged(int32 Value){}
void JoystickDelegate::POV1Changed(int32 Value){}
void JoystickDelegate::Slider1Changed(int32 Value){}


/** Availability */
bool JoystickDelegate::JoystickIsAvailable()
{
	//hydra will always have an enabled count of 2 when plugged in and working, stem functionality undefined.
	return _joystickLatestData->enabled;
}

/*
// Call to determine which hand you're holding the controller in. Determine by last docking position.
HydraControllerHand HydraDelegate::HydraWhichHand(int32 controller)
{
	return (HydraControllerHand)HydraLatestData->controllers[controller].which_hand;
}
*/
/** Poll for latest data.*/


joystickControllerDataUE* JoystickDelegate::JoystickGetLatestData()
{
	return _joystickLatestData;
}


void JoystickDelegate::JoystickStartup()
{
	if (IJoystickPlugin::IsAvailable())
	{
		//Required to Work - Set self as a delegate
		IJoystickPlugin::Get().SetDelegate((JoystickDelegate*)this);
	}
}
void JoystickDelegate::JoystickTick(float DeltaTime)
{
	if (IJoystickPlugin::IsAvailable())
	{
		//Required to Work - This is the plugin magic
		IJoystickPlugin::Get().JoystickTick(DeltaTime);
	}
}
