#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"

UJoystickSingleController::UJoystickSingleController(const FObjectInitializer& ObjectInitializer) : UObject(ObjectInitializer)
{
}

int32 UJoystickSingleController::Player()
{
	return player;
}

int64 UJoystickSingleController::ButtonsPressedLow()
{
	return data.buttonsPressedL;
}

int64 UJoystickSingleController::ButtonsPressedHigh()
{
	return data.buttonsPressedH;
}

bool UJoystickSingleController::ButtonPressed(int32 number)
{
	if (number < 1 || number > 128) return false;
	number--;
	return (number < 64
		? data.buttonsPressedL & (uint64(1) << number)
		: data.buttonsPressedH & (uint64(1) << (number - 64))) != 0;
}

FVector UJoystickSingleController::Axis() { return data.Axis; }

FVector UJoystickSingleController::RAxis() { return data.RAxis; }

TEnumAsByte<JoystickPOVDirection> UJoystickSingleController::POV(int32 index)
{
	if (index < 1 || index > 4) return DIRECTION_NONE;
	return data.POV[index - 1];
}

FVector2D UJoystickSingleController::POVAxis(int32 index)
{
	if (index < 1 || index > 4) return FVector2D(0, 0);
	return POVAxis(data.POV[index - 1]);
}

FVector2D UJoystickSingleController::Slider() { return data.Slider; }

bool UJoystickSingleController::IsConnected() { return info.Connected; }

FGuid UJoystickSingleController::ProductId()
{
	FGuid guid;
	memcpy(&guid, &info.ProductId, sizeof(FGuid));
	return guid;
}

FName UJoystickSingleController::ProductName() { return info.ProductName; }

FName UJoystickSingleController::InstanceName() { return info.InstanceName; }

void UJoystickSingleController::Init(int player, const JoystickData &data, const JoystickInfo &info)
{
	this->player = player;
	this->data = data;
	this->info = info;
}
