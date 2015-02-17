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

	if (m_DeviceSDL != nullptr) {
		UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to add devices..."));

		for (int iDevice = 0; iDevice < m_DeviceSDL->getNumberOfDevices(); iDevice++) {
			g_HotPlugDelegate->JoystickPluggedIn(iDevice);
		}
	}
	
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

		for (int iDevice = 0; iDevice < numJoysticks; iDevice++) {
			RemoveInputDevice(iDevice);
			g_HotPlugDelegate->JoystickUnplugged(iDevice);
		}

		delete m_DeviceSDL;
		m_DeviceSDL = NULL;
	}

}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::AddInputDevice(int iDevice)
{
	bool result = false;

	sDeviceInfoSDL deviceInfoSDL;

	if (m_InputDevices.IsValidIndex(iDevice)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("already an registered device %s %i"), *m_InputDevices[iDevice].DeviceName, iDevice);
	} else
	if (m_DeviceSDL->getDeviceSDL(iDevice, deviceInfoSDL)) {
		FJoystickInfo deviceInfo;

		deviceInfo.Connected = deviceInfoSDL.isConnected;
		deviceInfo.JoystickIndex = iDevice;

		deviceInfo.InstanceId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductName = FName(*m_DeviceSDL->getDeviceName(iDevice));
		deviceInfo.InstanceName = FName(*m_DeviceSDL->getDeviceName(iDevice));

		FString strDeviceName = deviceInfo.InstanceName.ToString().Replace(TEXT(" "), TEXT(""));
		deviceInfo.DeviceName = strDeviceName;

		deviceInfo.IsGameController = deviceInfoSDL.isGameController;		

		UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *strDeviceName, iDevice);
		m_InputDevices.Add(deviceInfo);

		FJoystickState newDeviceState(iDevice);
		if (m_DeviceSDL->getDeviceState(newDeviceState, m_InputDevices[iDevice], iDevice)) {

			// create FKeyDetails for axis
			for (int iAxis = 0; iAxis < newDeviceState.NumberOfAxis; iAxis++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceAxis", "Device {0} Axis {1}"), FText::AsNumber(iDevice), FText::AsNumber(iAxis));

				FString strName("Axis");
				strName.Append(FString::FromInt(iAxis));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *textValue.ToString(), iDevice);

				g_DeviceAxisKeys[iDevice].Add(FKey(FName(*strName)));

				EKeys::AddKey(FKeyDetails(g_DeviceAxisKeys[iDevice][iAxis], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

			}

			// create FKeyDetails for buttons
			for (int iButton = 0; iButton < newDeviceState.NumberOfButtons; iButton++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceButton", "Device {0} Button {1}"), FText::AsNumber(iDevice), FText::AsNumber(iButton));

				FString strName("Button");
				strName.Append(FString::FromInt(iButton));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s"), *textValue.ToString());

				g_DeviceButtonKeys[iDevice].Add(FKey(FName(*strName)));

				EKeys::AddKey(FKeyDetails(g_DeviceButtonKeys[iDevice][iButton], textValue, FKeyDetails::GamepadKey));
			}

			// create FKeyDetails for hats
			for (int iHat = 0; iHat < newDeviceState.NumberOfHats; iHat++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceHat", "Device {0} Hat {1}"), FText::AsNumber(iDevice), FText::AsNumber(iHat));

				FString strName("Hat");
				strName.Append(FString::FromInt(iHat));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add %s"), *textValue.ToString());

				g_DeviceHatKeys[iDevice].Add(FKey(FName(*strName)));

				EKeys::AddKey(FKeyDetails(g_DeviceHatKeys[iDevice][iHat], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
			}

			// create FKeyDetails for balls
			for (int iBall = 0; iBall < newDeviceState.NumberOfBalls; iBall++) {
				//FText textValue = FText::Format(LOCTEXT("DeviceBall", "Device {0} Ball {1}"), FText::AsNumber(iDevice), FText::AsNumber(iBall));

				FString strName("Ball");
				strName.Append(FString::FromInt(iBall));
				strName.Append("_");
				strName.Append(strDeviceName);

				FText textValue = FText::FromString(strName);

				UE_LOG(JoystickPluginLog, Log, TEXT("add Ball/Slider: %s"), *textValue.ToString());

				g_DeviceBallKeys[iDevice].Add(FKey(FName(*strName)));

				EKeys::AddKey(FKeyDetails(g_DeviceBallKeys[iDevice][iBall], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
			}

			prevData.Add(newDeviceState);
			currData.Add(newDeviceState);
		}
	}
	

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool FJoystickPlugin::RemoveInputDevice(int iDevice)
{
	bool result = false;
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::RemoveDevice() not implemented"));
	if (m_InputDevices.IsValidIndex(iDevice)) {
		m_InputDevices.RemoveAt(iDevice, 1);
	}
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

		joystickDelegate->Joysticks = m_InputDevices;
	
		if (m_DeviceSDL != nullptr) {
			UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to add devices..."));

			for (int iDevice = 0; iDevice < m_DeviceSDL->getNumberOfDevices(); iDevice++) {
				AddInputDevice(iDevice);
			}
		}
		//for (FJoystickInfo &joystick : m_InputDevice) {
		//	//@BUG this is never called for sdl version
		//	joystickDelegate->JoystickPluggedIn(joystick.JoystickIndex);
		//}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void FJoystickPlugin::JoystickPluggedIn(int32 iDevice)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), iDevice);

	if (m_DeviceSDL == NULL) {
		return;
	}

	sDeviceInfoSDL deviceInfoSDL;
	if (m_DeviceSDL->initDevice(iDevice, deviceInfoSDL)) {
		if (deviceInfoSDL.isConnected) {
			if (AddInputDevice(iDevice)) {
				UE_LOG(JoystickPluginLog, Log, TEXT("	SUCCESS add device %i"), iDevice);
			}
			else {
				UE_LOG(JoystickPluginLog, Log, TEXT("	FAILED add device %i"), iDevice);
			}
		}

	} else {
		UE_LOG(JoystickPluginLog, Log, TEXT("!!WARNING!! no device %i found"), iDevice);
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

void FJoystickPlugin::JoystickUnplugged(int32 iDevice)
{
	sDeviceInfoSDL deviceInfo;
	
	m_DeviceSDL->getNumberOfDevices();

	if (m_DeviceSDL->getDeviceSDL(iDevice, deviceInfo)) {
		if (deviceInfo.isConnected) {
			m_DeviceSDL->doneDevice(deviceInfo);
		}
	}

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

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
	m_DeviceSDL->update(DeltaTime);
	
	for (int iDevice = 0; iDevice < m_InputDevices.Num(); iDevice++) {
		if (m_InputDevices.IsValidIndex(iDevice)) {
			if (m_InputDevices[iDevice].Connected) {
				//get the freshest data
				FJoystickState newDeviceData(iDevice);
				if (m_DeviceSDL->getDeviceState(newDeviceData, m_InputDevices[iDevice], iDevice)) {
					prevData[iDevice] = currData[iDevice];
					currData[iDevice] = newDeviceData;
				}
			}
		}
	}

	DelegateTick(DeltaTime);
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

/** Internal Tick - Called by the Plugin */
void FJoystickPlugin::DelegateTick(float DeltaTime)
{

	//Update delegate
	if (joystickDelegate) {
		joystickDelegate->PreviousFrame = joystickDelegate->LatestFrame;
		joystickDelegate->LatestFrame = currData;		
	}

	for (int32 iDevice = 0; iDevice < m_InputDevices.Num(); iDevice++)
	{
		auto const & current = currData[iDevice];
		auto const & prev = prevData[iDevice];

		// check SDL buttons
		for (uint64 i = 0; i < current.NumberOfButtons; i++) {			
			if (g_DeviceButtonKeys[iDevice].IsValidIndex(i)) {
				if (current.ButtonsArray[i] == 1) {
					EmitKeyDownEventForKey(g_DeviceButtonKeys[iDevice][i], iDevice, false);
					if (joystickDelegate) {
						joystickDelegate->JoystickButtonPressed(i, current);
						joystickDelegate->ButtonsArrayChanged(i, true, current);
					}
				}
				else
					if (prev.ButtonsArray[i] == 1) {
						EmitKeyUpEventForKey(g_DeviceButtonKeys[iDevice][i], iDevice, false);
						if (joystickDelegate) {
							joystickDelegate->JoystickButtonReleased(i, current);
							joystickDelegate->ButtonsArrayChanged(i, false, current);
						}
					}
			}
		}
		
		//check axis
		for (uint64 i = 0; i < current.NumberOfAxis; i++) {
			if (g_DeviceAxisKeys[iDevice].IsValidIndex(i)) {
				if (current.AxisArray[i] != prev.AxisArray[i]) {
					if (joystickDelegate) {
						joystickDelegate->AxisArrayChanged(i, current.AxisArray[i], prev.AxisArray[i], current, prev);
					}

					EmitAnalogInputEventForKey(g_DeviceAxisKeys[iDevice][i], current.AxisArray[i], iDevice, 0);
				}
			}
		}

		//check hats
		for (uint64 i = 0; i < current.NumberOfHats; i++) {
			if (g_DeviceHatKeys[iDevice].IsValidIndex(i)) {
				if (current.HatsArray[i] != prev.HatsArray[i]) {
					if (joystickDelegate) {
						joystickDelegate->HatsArrayChanged(i, current.HatsArray[i], current);
					}

					EmitAnalogInputEventForKey(g_DeviceHatKeys[iDevice][i], current.HatsArray[i], iDevice, 0);
				}
			}
		}

		//check balls
		for (uint64 i = 0; i < current.NumberOfBalls; i++) {
			if (g_DeviceBallKeys[iDevice].IsValidIndex(i)) {
				if (current.BallsArray[i] != prev.BallsArray[i]) {
					if (joystickDelegate) {
						joystickDelegate->BallsArrayChanged(i, current.BallsArray[i], current);
					}

					EmitAnalogInputEventForKey(g_DeviceBallKeys[iDevice][i], current.BallsArray[i], iDevice, 0);
				}
			}
		}

		////check slider
		//if (current.Slider != prev.Slider)
		//{
		//	if (joystickDelegate)
		//		joystickDelegate->SliderChanged(current.Slider, current);
		//	EmitAnalogInputEventForKey(EKeysJoystick::JoystickSlider[0], current.Slider.X, p, 0);
		//	EmitAnalogInputEventForKey(EKeysJoystick::JoystickSlider[1], current.Slider.Y, p, 0);
		//}
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
