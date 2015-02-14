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
