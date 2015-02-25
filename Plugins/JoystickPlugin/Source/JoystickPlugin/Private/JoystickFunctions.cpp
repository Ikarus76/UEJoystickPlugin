
#include "JoystickPluginPrivatePCH.h"
#include "JoystickFunctions.h"
#include "IJoystickPlugin.h"
#include "JoystickInterface.h"
#include "JoystickDevice.h"
#include "JoystickPlugin.h"

UJoystickFunctions::UJoystickFunctions(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}

FVector2D UJoystickFunctions::POVAxis(EJoystickPOVDirection direction)
{
	return ::POVAxis(direction);
}

FJoystickInfo UJoystickFunctions::GetJoystick(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickInfo();

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->InputDevices.Contains(FDeviceId(deviceId)))
		return FJoystickInfo();

	return device->InputDevices[FDeviceId(deviceId)];
}

FJoystickState UJoystickFunctions::GetJoystickState(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickState();

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->InputDevices.Contains(FDeviceId(deviceId)))
		return FJoystickState();

	return device->CurrentState[FDeviceId(deviceId)];
}

FJoystickState UJoystickFunctions::GetPreviousJoystickState(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickState();

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->InputDevices.Contains(FDeviceId(deviceId)))
		return FJoystickState();

	return device->PreviousState[FDeviceId(deviceId)];
}

int32 UJoystickFunctions::JoystickCount()
{
	if (!IJoystickPlugin::IsAvailable())
		return 0;

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	return device->InputDevices.Num();
}

bool UJoystickFunctions::RegisterForJoystickEvents(UObject* listener)
{
	if (!IJoystickPlugin::IsAvailable())
		return false;

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	return device->AddEventListener(listener);
}

bool UJoystickFunctions::MapJoystickDeviceToPlayer(int32 deviceId, int32 player)
{
	if (!IJoystickPlugin::IsAvailable())
		return false;

	TSharedPtr<FJoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->InputDevices.Contains(FDeviceId(deviceId)))
		return false;

	device->InputDevices[FDeviceId(deviceId)].Player = player;
	return true;
}
