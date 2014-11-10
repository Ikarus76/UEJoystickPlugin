#include "JoystickPluginPrivatePCH.h"

#define JOYSTICK_SCALING_FACTOR 65536
#define HALF_RANGE 32768
#define INVERSE_SCALING_FACTOR 0.00001525878	//this is 1/65536
#define JOYSTICK_FF_RANGE 10000
#define JOYSTICK_FF_HALF_RANGE JOYSTICK_FF_RANGE/2

DEFINE_LOG_CATEGORY(JoystickPluginLog);

FVector JoystickUtilityNormalizeAxis(FVector in)
{
	return ((in - HALF_RANGE)) / HALF_RANGE;
}

FVector2D JoystickUtilityNormalizeSlider(FVector2D value)
{
	return (value - HALF_RANGE) / HALF_RANGE;
}

float JoystickUtilityNormalizeForceValue(int32 value)
{
	return value/JOYSTICK_FF_RANGE;
}

int32 JoystickUtilityDeNormalizeForceValue(float value)
{
	return value*JOYSTICK_FF_RANGE;
}