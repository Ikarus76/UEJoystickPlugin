#pragma once

#include "JoystickPluginPrivatePCH.h"
#include "JoystickInterface.h"

UJoystickInterface::UJoystickInterface(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

//This is required for compiling, would also let you know if somehow you called
//the base event/function rather than the over-rided version
FString IJoystickInterface::ToString()
{
	return "IJoystickInterface::ToString()";
}
