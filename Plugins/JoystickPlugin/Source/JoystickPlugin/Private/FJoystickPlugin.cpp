#include "JoystickPluginPrivatePCH.h"

#include <SlateBasics.h>
#include <Text.h>

#include "IJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "FJoystickPlugin.h"

#include "DeviceSDL.h"

IMPLEMENT_MODULE(FJoystickPlugin, JoystickPlugin)

#define LOCTEXT_NAMESPACE "JoystickPlugin"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//@TODO create hey an function for creating the FKeyDetails for Axes, Button etc...

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//Init and Runtime
void FJoystickPlugin::StartupModule()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::StartupModule() creating Device SDL"));

	m_DeviceSDL = new DeviceSDL(this);

	// Trigger initial SDL_JOYDEVICEADDED events
	m_DeviceSDL->update(0);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::ShutdownModule()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::ShutdownModule()"));
	check(m_DeviceSDL != nullptr);

	UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to remove devices..."));

	for (auto const & device : m_InputDevices) {
		RemoveInputDevice(device.Key);
		JoystickUnplugged(device.Key);
	}

	delete m_DeviceSDL;
	m_DeviceSDL = nullptr;

}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::AddInputDevice(DeviceId iDevice)
{
	bool result = false;

	sDeviceInfoSDL deviceInfoSDL;

	if (m_InputDevices.Contains(iDevice)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("already an registered device %s %i"), *m_InputDevices[iDevice].DeviceName, static_cast<int>(iDevice));
	} else
	if (m_DeviceSDL->getDeviceSDL(iDevice, deviceInfoSDL)) {
		FJoystickInfo deviceInfo;

		deviceInfo.Connected = deviceInfoSDL.isConnected;
		deviceInfo.DeviceId = static_cast<int>(iDevice);
		deviceInfo.Player = 0;

		deviceInfo.ProductId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductName = *m_DeviceSDL->getDeviceName(iDevice);
		deviceInfo.DeviceName = deviceInfo.ProductName.Replace(TEXT(" "), TEXT(""));

		deviceInfo.IsGameController = deviceInfoSDL.isGameController;		

		UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *deviceInfo.DeviceName, static_cast<int>(iDevice));
		m_InputDevices.Add(iDevice, deviceInfo);

		FJoystickState newDeviceState(static_cast<int>(iDevice));
		if (m_DeviceSDL->getDeviceState(newDeviceState, m_InputDevices[iDevice], iDevice)) {

			// create FKeyDetails for axis
			deviceAxisKeys.Add(iDevice);
			for (int iAxis = 0; iAxis < newDeviceState.NumberOfAxis; iAxis++)
			{
				FString strName = FString::Printf(TEXT("Joystick_%s_Axis%d"), *deviceInfo.DeviceName, iAxis);
				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, static_cast<int>(iDevice));
				deviceAxisKeys[iDevice].Add(FKey(FName(*strName)));

				if (!EKeys::GetKeyDetails(deviceAxisKeys[iDevice][iAxis]).IsValid())
				{
					FText textValue = FText::Format(LOCTEXT("DeviceAxis", "{0} Axis {1}"), FText::FromString(deviceInfo.ProductName), FText::AsNumber(iAxis));
					EKeys::AddKey(FKeyDetails(deviceAxisKeys[iDevice][iAxis], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
				}
			}

			// create FKeyDetails for buttons
			deviceButtonKeys.Add(iDevice);
			for (int iButton = 0; iButton < newDeviceState.NumberOfButtons; iButton++)
			{
				FString strName = FString::Printf(TEXT("Joystick_%s_Button%d"), *deviceInfo.DeviceName, iButton);
				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, static_cast<int>(iDevice));
				deviceButtonKeys[iDevice].Add(FKey(FName(*strName)));

				if (!EKeys::GetKeyDetails(deviceButtonKeys[iDevice][iButton]).IsValid())
				{
					FText textValue = FText::Format(LOCTEXT("DeviceButton", "{0} Button {1}"), FText::FromString(deviceInfo.ProductName), FText::AsNumber(iButton));
					EKeys::AddKey(FKeyDetails(deviceButtonKeys[iDevice][iButton], textValue, FKeyDetails::GamepadKey));
				}
			}

			FString _2DaxisNames[] = { TEXT("X"), TEXT("Y") };

			// create FKeyDetails for hats
			for (int iAxis = 0; iAxis < 2; iAxis++)
			{
				deviceHatKeys[iAxis].Add(iDevice);
				for (int iHat = 0; iHat < newDeviceState.NumberOfHats; iHat++)
				{
					FString strName = FString::Printf(TEXT("Joystick_%s_Hat%d_%s"), *deviceInfo.DeviceName, iHat, *_2DaxisNames[iAxis]);
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, static_cast<int>(iDevice));
					FKey key { *strName };
					deviceHatKeys[iAxis][iDevice].Add(key);

					if (!EKeys::GetKeyDetails(key).IsValid())
					{
						FText textValue = FText::Format(LOCTEXT("DeviceHat", "{0} Hat {1} {2}"), FText::FromString(deviceInfo.ProductName), FText::AsNumber(iHat), FText::FromString(_2DaxisNames[iAxis]));
						EKeys::AddKey(FKeyDetails(key, textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
					}
				}
			}

			/*// create FKeyDetails for balls
			for (int iAxis = 0; iAxis < 2; iAxis++)
			{
				deviceBallKeys[iAxis].Add(iDevice);
				for (int iBall = 0; iBall < newDeviceState.NumberOfBalls; iBall++)
				{
					FString strName = FString::Printf(TEXT("Joystick_%s_Ball%d_%s"), *strDeviceName, iBall, _2DaxisNames[iAxis]);
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, static_cast<int>(iDevice));
					FKey key{ *strName };
					deviceBallKeys[iAxis][iDevice].Add(key);

					if (!EKeys::GetKeyDetails(key).IsValid())
					{
						FText textValue = FText::Format(LOCTEXT("DeviceBall", "{0} Ball {1} {2}"), FText::FromString(strDeviceName), FText::AsNumber(iBall), FText::FromString(_2DaxisNames[iAxis]));
						EKeys::AddKey(FKeyDetails(key, textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
					}
				}
			}*/

			prevData.Add(iDevice, newDeviceState);
			currData.Add(iDevice, newDeviceState);
		}
	}
	

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::RemoveInputDevice(DeviceId iDevice)
{
	bool result = false;
	/*UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::RemoveDevice() not implemented"));
	if (m_InputDevices.IsValidIndex(iDevice)) {
		m_InputDevices.RemoveAt(iDevice, 1);
	}*/
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//Public API Implementation

/** Public API - Required **/

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::SetDelegate(JoystickDelegate* newDelegate)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::SetDelegate"));

	joystickDelegate = newDelegate;
	if (joystickDelegate) {		

		m_InputDevices.GenerateValueArray(joystickDelegate->Joysticks);
	
		/*if (m_DeviceSDL != nullptr) {
			UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to add devices..."));

			for (int iDevice = 0; iDevice < m_DeviceSDL->getNumberOfDevices(); iDevice++) {
				AddInputDevice(iDevice);
			}
		}*/
		//for (FJoystickInfo &joystick : m_InputDevice) {
		//	//@BUG this is never called for sdl version
		//	joystickDelegate->JoystickPluggedIn(joystick.JoystickIndex);
		//}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickPluggedIn(DeviceIndex iDevice)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), static_cast<int>(iDevice));
	check(m_DeviceSDL != nullptr);

	sDeviceInfoSDL deviceInfoSDL;
	if (m_DeviceSDL->initDevice(iDevice, deviceInfoSDL)) {
		if (deviceInfoSDL.isConnected) {
			if (AddInputDevice(deviceInfoSDL.deviceId)) {
				UE_LOG(JoystickPluginLog, Log, TEXT("	SUCCESS add device %i"), static_cast<int>(iDevice));
			}
			else {
				UE_LOG(JoystickPluginLog, Log, TEXT("	FAILED add device %i"), static_cast<int>(iDevice));
			}
		}

	} else {
		UE_LOG(JoystickPluginLog, Log, TEXT("!!WARNING!! no device %i found"), static_cast<int>(iDevice));
	}

	//UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn %d"), joystick.Player);

	//// First try to find the same joystick if it was connected before (to get the same index)
	//joystick.Player = m_Joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.InstanceId == joystick.InstanceId; });
	//if (joystick.Player == INDEX_NONE)
	//{
	//	// Otherwise try to find an unused slot
	//	joystick.Player = m_Joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.Connected == false; });
	//}

	//if (joystick.Player == INDEX_NONE)
	//{
	//	// Finally add a now slot
	//	joystick.Player = m_Joysticks.Add(joystick);
	//	m_Joysticks[joystick.Player].Player = joystick.Player;
	//	prevData.Add(FJoystickState(joystick.Player));
	//	currData.Add(FJoystickState(joystick.Player));
	//}
	//else
	//{
	//	m_Joysticks[joystick.Player] = joystick;
	//}

	//if (joystickDelegate)
	//{
	//	joystickDelegate->Joysticks = m_Joysticks;
	//	joystickDelegate->JoystickPluggedIn(joystick);
	//}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickUnplugged(DeviceId iDevice)
{
	check(m_DeviceSDL != nullptr);
	m_DeviceSDL->doneDevice(iDevice);

	//int player = m_Joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.InstanceId == id; });
	//if (player == INDEX_NONE)
	//{
	//	// Can happen e.g. if we fail to acquire a joystick
	//	return;
	//}

	//m_Joysticks[player].Connected = false;

	//DelegateTick(0);
	//UE_LOG(JoystickPluginLog, Log, TEXT("Joystick for player %d disconnected"), player);

	//if (joystickDelegate)
	//{
	//	// Let joystick state return to zero
	//	joystickDelegate->JoystickUnplugged(m_Joysticks[player]);
	//}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//UE v4.6 IM event wrappers
bool EmitKeyUpEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitKeyDownEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitAnalogInputEventForKey(FKey key, float value, int32 user, bool repeat)
{
	FAnalogInputEvent AnalogInputEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0, value);
	return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitPointerEventForKey(int32 pointerIndex, const FVector2D &value)
{
	FPointerEvent pointerEvent(pointerIndex, FVector2D::ZeroVector, FVector2D::ZeroVector, value, TSet<FKey>(), FSlateApplication::Get().GetModifierKeys());
	return FSlateApplication::Get().ProcessMouseMoveEvent(pointerEvent);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickButton(DeviceId iDevice, int32 button, bool pressed) 
{
	currData[iDevice].ButtonsArray[button] = pressed;
	if (pressed)
		EmitKeyDownEventForKey(deviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);
	else
		EmitKeyUpEventForKey(deviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);

	if (joystickDelegate) {
		joystickDelegate->JoystickButtonPressed(button, currData[iDevice]);
	}
}

void FJoystickPlugin::JoystickAxis(DeviceId iDevice, int32 axis, float value)
{
	currData[iDevice].AxisArray[axis] = value;
	EmitAnalogInputEventForKey(deviceAxisKeys[iDevice][axis], value, m_InputDevices[iDevice].Player, 0);

	if (joystickDelegate) {
		joystickDelegate->AxisArrayChanged(axis, currData[iDevice].AxisArray[axis], prevData[iDevice].AxisArray[axis], currData[iDevice], prevData[iDevice]);
	}
}

void FJoystickPlugin::JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value)
{
	currData[iDevice].HatsArray[hat] = value;

	FVector2D povAxis = POVAxis(value);
	EmitAnalogInputEventForKey(deviceHatKeys[0][iDevice][hat], povAxis.X, m_InputDevices[iDevice].Player, 0);
	EmitAnalogInputEventForKey(deviceHatKeys[1][iDevice][hat], povAxis.Y, m_InputDevices[iDevice].Player, 0);

	if (joystickDelegate) {
		joystickDelegate->HatsArrayChanged(hat, value, currData[iDevice]);
	}
}

void FJoystickPlugin::JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy)
{
	// EmitPointerEventForKey(ball, FVector2D(dx, dy)); TODO: Test how this works with an actual "ball"

	if (joystickDelegate) {
		joystickDelegate->BallsArrayChanged(ball, dx, dy, currData[iDevice]);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
	check(m_DeviceSDL != nullptr);

	for (auto & device : m_InputDevices) {
		if (m_InputDevices.Contains(device.Key)) {
			if (m_InputDevices[device.Key].Connected) {
				prevData[device.Key] = currData[device.Key];
			}
		}
	}

	m_DeviceSDL->update(DeltaTime);

	if (joystickDelegate) {
		joystickDelegate->PreviousFrame = joystickDelegate->LatestFrame;
		currData.GenerateValueArray(joystickDelegate->LatestFrame);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::ForceFeedbackXY(int32 x, int32 y, float magnitudeScale)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("Force feedback currently not implemented correctly, aborting."));
	return;

	//if (!g_pJoystickFF){
	//	UE_LOG(JoystickPluginLog, Log, TEXT("Force feedback not available."));
	//	return;
	//}

	////scale the input to joystick scaling
	//SetForceFeedbackXY(x, y, magnitudeScale);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::JoystickIsAvailable()
{
	check(m_DeviceSDL != nullptr);
	bool result = false;

	//UE_LOG(JoystickPluginLog, Log, TEXT("check is JoystickIsAvailable"));

	if (m_DeviceSDL->getNumberOfDevices() > 0) {
		result = true;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
