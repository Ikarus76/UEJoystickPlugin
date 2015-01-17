
#include "JoystickPluginPrivatePCH.h"
#include "JoystickFunctions.h"

UJoystickFunctions::UJoystickFunctions(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FVector2D UJoystickFunctions::POVAxis(TEnumAsByte<JoystickPOVDirection> direction)
{
	return ::POVAxis(direction);
}

bool UJoystickFunctions::ButtonPressed(FJoystickState state, int32 number)
{
	if (number < 1 || number > 128) return false;
	number--;
	return (number < 64
		? state.buttonsPressedL & (uint64(1) << number)
		: state.buttonsPressedH & (uint64(1) << (number - 64))) != 0;
}
