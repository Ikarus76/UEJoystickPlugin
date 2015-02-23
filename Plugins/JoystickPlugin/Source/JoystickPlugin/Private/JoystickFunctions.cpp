
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

FJoystickInfo UJoystickFunctions::GetJoystick(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickInfo();

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->m_InputDevices.Contains(DeviceId(deviceId)))
		return FJoystickInfo();

	return device->m_InputDevices[DeviceId(deviceId)];
}

FJoystickState UJoystickFunctions::GetJoystickState(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickState();

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->m_InputDevices.Contains(DeviceId(deviceId)))
		return FJoystickState();

	return device->currData[DeviceId(deviceId)];
}

FJoystickState UJoystickFunctions::GetPreviousJoystickState(int32 deviceId)
{
	if (!IJoystickPlugin::IsAvailable())
		return FJoystickState();

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->m_InputDevices.Contains(DeviceId(deviceId)))
		return FJoystickState();

	return device->prevData[DeviceId(deviceId)];
}

int32 UJoystickFunctions::JoystickCount()
{
	if (!IJoystickPlugin::IsAvailable())
		return 0;

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	return device->m_InputDevices.Num();
}

bool UJoystickFunctions::RegisterForJoystickEvents(UObject* listener)
{
	if (!IJoystickPlugin::IsAvailable())
		return false;

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	return device->AddEventListener(listener);
}

bool UJoystickFunctions::MapJoystickDeviceToPlayer(int32 deviceId, int32 player)
{
	if (!IJoystickPlugin::IsAvailable())
		return false;

	TSharedPtr<JoystickDevice> device = static_cast<FJoystickPlugin&>(IJoystickPlugin::Get()).JoystickDevice;
	if (!device->m_InputDevices.Contains(DeviceId(deviceId)))
		return false;

	device->m_InputDevices[DeviceId(deviceId)].Player = player;
	return true;
}
