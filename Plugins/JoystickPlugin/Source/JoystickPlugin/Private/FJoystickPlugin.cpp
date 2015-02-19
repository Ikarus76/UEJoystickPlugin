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

	m_DeviceSDL = new DeviceSDL();
	
	g_HotPlugDelegate = this;

	/*if (m_DeviceSDL != nullptr) {
		UE_LOG(JoystickPluginLog, Log, TEXT("Add already connected devices"));

		for (int iDevice = 0; iDevice < m_DeviceSDL->getNumberOfDevices(); iDevice++) {
			g_HotPlugDelegate->JoystickPluggedIn(static_cast<DeviceIndex>(iDevice));
		}
	}*/
	
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::ShutdownModule()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::ShutdownModule()"));

	if (m_DeviceSDL != NULL) {
		int numJoysticks = m_DeviceSDL->getNumberOfDevices();

		UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to remove devices..."));

		for (auto const & device : m_InputDevices) {
			RemoveInputDevice(device.Key);
			g_HotPlugDelegate->JoystickUnplugged(device.Key);
		}

		delete m_DeviceSDL;
		m_DeviceSDL = NULL;
	}

}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::AddInputDevice(DeviceId iDevice)
{
	bool result = false;

	sDeviceInfoSDL deviceInfoSDL;

	if (m_InputDevices.Contains(iDevice)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("already an registered device %s %i"), *m_InputDevices[iDevice].DeviceName, (int)iDevice);
	} else
	if (m_DeviceSDL->getDeviceSDL(iDevice, deviceInfoSDL)) {
		FJoystickInfo deviceInfo;

		deviceInfo.Connected = deviceInfoSDL.isConnected;
		deviceInfo.DeviceId = (int)iDevice;
		deviceInfo.Player = 0;

		deviceInfo.InstanceId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductName = FName(*m_DeviceSDL->getDeviceName(iDevice));
		deviceInfo.InstanceName = FName(*m_DeviceSDL->getDeviceName(iDevice));

		FString strDeviceName = deviceInfo.InstanceName.ToString().Replace(TEXT(" "), TEXT(""));
		deviceInfo.DeviceName = strDeviceName;

		deviceInfo.IsGameController = deviceInfoSDL.isGameController;		

		UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *strDeviceName, (int)iDevice);
		m_InputDevices.Add(iDevice, deviceInfo);

		FJoystickState newDeviceState((int)iDevice);
		if (m_DeviceSDL->getDeviceState(newDeviceState, m_InputDevices[iDevice], iDevice)) {

			// create FKeyDetails for axis
			g_DeviceAxisKeys.Add(iDevice);
			for (int iAxis = 0; iAxis < newDeviceState.NumberOfAxis; iAxis++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceAxis", "Device {0} Axis {1}"), FText::AsNumber(iDevice), FText::AsNumber(iAxis));

				FString strName("Axis");
				strName.Append(FString::FromInt(iAxis));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *textValue.ToString(), (int)iDevice);

				g_DeviceAxisKeys[iDevice].Add(FKey(FName(*strName)));
				if (!EKeys::GetKeyDetails(g_DeviceAxisKeys[iDevice][iAxis]).IsValid())
				{
					EKeys::AddKey(FKeyDetails(g_DeviceAxisKeys[iDevice][iAxis], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
				}
			}

			// create FKeyDetails for buttons
			g_DeviceButtonKeys.Add(iDevice);
			for (int iButton = 0; iButton < newDeviceState.NumberOfButtons; iButton++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceButton", "Device {0} Button {1}"), FText::AsNumber(iDevice), FText::AsNumber(iButton));

				FString strName("Button");
				strName.Append(FString::FromInt(iButton));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s"), *textValue.ToString());

				g_DeviceButtonKeys[iDevice].Add(FKey(FName(*strName)));

				if (!EKeys::GetKeyDetails(g_DeviceButtonKeys[iDevice][iButton]).IsValid())
				{
					EKeys::AddKey(FKeyDetails(g_DeviceButtonKeys[iDevice][iButton], textValue, FKeyDetails::GamepadKey));
				}
			}

			// create FKeyDetails for hats
			g_DeviceHatKeys.Add(iDevice);
			for (int iHat = 0; iHat < newDeviceState.NumberOfHats; iHat++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceHat", "Device {0} Hat {1}"), FText::AsNumber(iDevice), FText::AsNumber(iHat));

				FString strName("Hat");
				strName.Append(FString::FromInt(iHat));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s"), *textValue.ToString());

				g_DeviceHatKeys[iDevice].Add(FKey(FName(*strName)));

				if (!EKeys::GetKeyDetails(g_DeviceHatKeys[iDevice][iHat]).IsValid())
				{
					EKeys::AddKey(FKeyDetails(g_DeviceHatKeys[iDevice][iHat], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
				}
			}

			// create FKeyDetails for balls
			//g_DeviceBallKeys.Add(iDevice);
			//for (int iBall = 0; iBall < newDeviceState.NumberOfBalls; iBall++) {
			//	//FText textValue = FText::Format(LOCTEXT("DeviceBall", "Device {0} Ball {1}"), FText::AsNumber(iDevice), FText::AsNumber(iBall));

			//	FString strName("Ball");
			//	strName.Append(FString::FromInt(iBall));
			//	strName.Append("_");
			//	strName.Append(strDeviceName);

			//	FText textValue = FText::FromString(strName);

			//	UE_LOG(JoystickPluginLog, Log, TEXT("add Ball/Slider: %s"), *textValue.ToString());

			//	g_DeviceBallKeys[iDevice].Add(FKey(FName(*strName)));

			//	EKeys::AddKey(FKeyDetails(g_DeviceBallKeys[iDevice][iBall], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
			//}

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
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), (int)iDevice);

	if (m_DeviceSDL == NULL) {
		return;
	}

	sDeviceInfoSDL deviceInfoSDL;
	if (m_DeviceSDL->initDevice(iDevice, deviceInfoSDL)) {
		if (deviceInfoSDL.isConnected) {
			if (AddInputDevice(deviceInfoSDL.deviceId)) {
				UE_LOG(JoystickPluginLog, Log, TEXT("	SUCCESS add device %i"), (int)iDevice);
			}
			else {
				UE_LOG(JoystickPluginLog, Log, TEXT("	FAILED add device %i"), (int)iDevice);
			}
		}

	} else {
		UE_LOG(JoystickPluginLog, Log, TEXT("!!WARNING!! no device %i found"), (int)iDevice);
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

void FJoystickPlugin::JoystickButton(DeviceId iDevice, int32 button, bool pressed) 
{
	currData[iDevice].ButtonsArray[button] = pressed;
	if (pressed)
		EmitKeyDownEventForKey(g_DeviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);
	else
		EmitKeyUpEventForKey(g_DeviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);

	if (joystickDelegate) {
		joystickDelegate->JoystickButtonPressed(button, currData[iDevice]);
	}
}

void FJoystickPlugin::JoystickAxis(DeviceId iDevice, int32 axis, float value)
{
	currData[iDevice].AxisArray[axis] = value;
	EmitAnalogInputEventForKey(g_DeviceAxisKeys[iDevice][axis], value, m_InputDevices[iDevice].Player, 0);

	if (joystickDelegate) {
		joystickDelegate->AxisArrayChanged(axis, currData[iDevice].AxisArray[axis], prevData[iDevice].AxisArray[axis], currData[iDevice], prevData[iDevice]);
	}
}

void FJoystickPlugin::JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value)
{
	currData[iDevice].HatsArray[hat] = value;

	EmitAnalogInputEventForKey(g_DeviceHatKeys[iDevice][hat], value, m_InputDevices[iDevice].Player, 0); //TODO: two axis direction instead, like before?

	if (joystickDelegate) {
		joystickDelegate->HatsArrayChanged(hat, value, currData[iDevice]);
	}
}

void FJoystickPlugin::JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy)
{
	//check balls
	if (joystickDelegate) {
		joystickDelegate->BallsArrayChanged(ball, dx, dy, currData[iDevice]);
	}

	// TODO: Some kind of mouse event maybe? EmitAnalogInputEventForKey(g_DeviceBallKeys[iDevice][i], current.BallsArray[i], device.Player, 0);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
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
