#include "JoystickPluginPrivatePCH.h"

#include <SlateBasics.h>
#include <Text.h>

#include "IJoystickPlugin.h"
#include "FJoystickPlugin.h"

#include "DeviceSDL.h"

IMPLEMENT_MODULE(FJoystickPlugin, JoystickPlugin)

#define LOCTEXT_NAMESPACE "JoystickPlugin"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

//Init and Runtime
JoystickDevice::JoystickDevice()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::StartupModule() creating Device SDL"));

	m_DeviceSDL = new DeviceSDL(this);

	// Trigger initial SDL_JOYDEVICEADDED events
	m_DeviceSDL->update();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

JoystickDevice::~JoystickDevice()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::ShutdownModule()"));
	check(m_DeviceSDL != nullptr);

	UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to remove devices..."));

	for (auto const & device : m_InputDevices) {
		JoystickUnplugged(device.Key);
	}

	delete m_DeviceSDL;
	m_DeviceSDL = nullptr;

}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool JoystickDevice::AddInputDevice(DeviceId iDevice)
{
	bool result = false;

	sDeviceInfoSDL deviceInfoSDL;

	if (m_InputDevices.Contains(iDevice)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("already an registered device %s %i"), *m_InputDevices[iDevice].DeviceName, iDevice.value);
	} else
	if (m_DeviceSDL->getDeviceSDL(iDevice, deviceInfoSDL)) {
		FJoystickInfo deviceInfo;

		deviceInfo.Connected = deviceInfoSDL.isConnected;
		deviceInfo.DeviceId = iDevice.value;
		deviceInfo.Player = 0;

		deviceInfo.ProductId = m_DeviceSDL->getDeviceGUIDtoGUID(iDevice);
		deviceInfo.ProductName = *m_DeviceSDL->getDeviceName(iDevice);
		deviceInfo.DeviceName = deviceInfo.ProductName.Replace(TEXT(" "), TEXT(""));

		deviceInfo.IsGameController = deviceInfoSDL.isGameController;		

		UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *deviceInfo.DeviceName, iDevice.value);
		m_InputDevices.Add(iDevice, deviceInfo);

		FJoystickState newDeviceState(iDevice.value);
		if (m_DeviceSDL->getDeviceState(newDeviceState, m_InputDevices[iDevice], iDevice)) {

			// create FKeyDetails for axis
			deviceAxisKeys.Add(iDevice);
			for (int iAxis = 0; iAxis < newDeviceState.NumberOfAxis; iAxis++)
			{
				FString strName = FString::Printf(TEXT("Joystick_%s_Axis%d"), *deviceInfo.DeviceName, iAxis);
				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, iDevice.value);
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
				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, iDevice.value);
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
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, iDevice.value);
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
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, iDevice.value);
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

//Public API Implementation

/** Public API - Required **/

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void JoystickDevice::JoystickPluggedIn(DeviceIndex deviceIndex)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), deviceIndex.value);
	check(m_DeviceSDL != nullptr);

	sDeviceInfoSDL deviceInfoSDL;
	if (m_DeviceSDL->initDevice(deviceIndex, deviceInfoSDL))
	{
		if (deviceInfoSDL.isConnected) 
		{
			if (AddInputDevice(deviceInfoSDL.deviceId))
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("	SUCCESS add device %i"), deviceInfoSDL.deviceId.value);
				for (auto & listener : eventListeners)
				{
					UObject * o = listener.Get();
					if (o != nullptr)
					{
						IJoystickInterface::Execute_JoystickPluggedIn(o, deviceInfoSDL.deviceId.value);
					}
				}
			}
			else
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("	FAILED add device %i"), deviceInfoSDL.deviceId.value);
			}
		}
	} 
	else 
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("!!WARNING!! no device %i found"), deviceIndex.value);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void JoystickDevice::JoystickUnplugged(DeviceId iDevice)
{
	check(m_DeviceSDL != nullptr);
	m_DeviceSDL->doneDevice(iDevice);

	m_InputDevices[iDevice].Connected = false;

	//DelegateTick(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("Joystick %d disconnected"), iDevice.value);

	for (auto & listener : eventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickUnplugged(o, iDevice.value);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitKeyUpEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
}

bool EmitKeyDownEventForKey(FKey key, int32 user, bool repeat)
{
	FKeyEvent KeyEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
}

void JoystickDevice::JoystickButton(DeviceId iDevice, int32 button, bool pressed) 
{
	currData[iDevice].ButtonsArray[button] = pressed;
	if (pressed)
		EmitKeyDownEventForKey(deviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);
	else
		EmitKeyUpEventForKey(deviceButtonKeys[iDevice][button], m_InputDevices[iDevice].Player, false);

	for (auto & listener : eventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			if (pressed)
				IJoystickInterface::Execute_JoystickButtonPressed(o, button, currData[iDevice]);
			else
				IJoystickInterface::Execute_JoystickButtonReleased(o, button, currData[iDevice]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitAnalogInputEventForKey(FKey key, float value, int32 user, bool repeat)
{
	FAnalogInputEvent AnalogInputEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0, value);
	return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
}

void JoystickDevice::JoystickAxis(DeviceId iDevice, int32 axis, float value)
{
	currData[iDevice].AxisArray[axis] = value;
	EmitAnalogInputEventForKey(deviceAxisKeys[iDevice][axis], value, m_InputDevices[iDevice].Player, 0);

	for (auto & listener : eventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickAxisArrayChanged(o, axis, currData[iDevice].AxisArray[axis], prevData[iDevice].AxisArray[axis], currData[iDevice], prevData[iDevice]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void JoystickDevice::JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value)
{
	currData[iDevice].HatsArray[hat] = value;

	FVector2D povAxis = POVAxis(value);
	EmitAnalogInputEventForKey(deviceHatKeys[0][iDevice][hat], povAxis.X, m_InputDevices[iDevice].Player, 0);
	EmitAnalogInputEventForKey(deviceHatKeys[1][iDevice][hat], povAxis.Y, m_InputDevices[iDevice].Player, 0);

	for (auto & listener : eventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickHatsArrayChanged(o, hat, value, currData[iDevice]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool EmitPointerEventForKey(int32 pointerIndex, const FVector2D &value)
{
	FPointerEvent pointerEvent(pointerIndex, FVector2D::ZeroVector, FVector2D::ZeroVector, value, TSet<FKey>(), FSlateApplication::Get().GetModifierKeys());
	return FSlateApplication::Get().ProcessMouseMoveEvent(pointerEvent);
}

void JoystickDevice::JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy)
{
	// EmitPointerEventForKey(ball, FVector2D(dx, dy)); TODO: Test how this works with an actual "ball"

	for (auto & listener : eventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickBallsArrayChanged(o, ball, dx, dy, currData[iDevice]);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// IInputDevice implementation
//////////////////////////////////////////////////////////////////////

void JoystickDevice::Tick(float DeltaTime)
{

}

void JoystickDevice::SendControllerEvents()
{
	check(m_DeviceSDL != nullptr);

	for (auto & device : m_InputDevices) {
		if (m_InputDevices.Contains(device.Key)) {
			if (m_InputDevices[device.Key].Connected) {
				prevData[device.Key] = currData[device.Key];
			}
		}
	}

	m_DeviceSDL->update();

	// Clean up weak references
	for (int i = 0; i < eventListeners.Num(); i++)
	{
		if (!eventListeners[i].IsValid())
		{
			eventListeners.RemoveAt(i);
			i--;
		}
	}
}

void JoystickDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
}

bool JoystickDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void JoystickDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	//FControllerState& ControllerState = ControllerStates[ControllerId];
	//if (ControllerState.bIsConnected)
	//{
	//	switch (ChannelType)
	//	{
	//	case FF_CHANNEL_LEFT_LARGE:
	//		ControllerState.ForceFeedback.LeftLarge = Value;
	//		break;
	//	case FF_CHANNEL_LEFT_SMALL:
	//		ControllerState.ForceFeedback.LeftSmall = Value;
	//		break;
	//	case FF_CHANNEL_RIGHT_LARGE:
	//		ControllerState.ForceFeedback.RightLarge = Value;
	//		break;
	//	case FF_CHANNEL_RIGHT_SMALL:
	//		ControllerState.ForceFeedback.RightSmall = Value;
	//		break;
	//	}
	//}
}

void JoystickDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values)
{
	//FControllerState& ControllerState = ControllerStates[ControllerId];
	//if (ControllerState.bIsConnected)
	//{
	//	ControllerState.ForceFeedback = Values;
	//}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool JoystickDevice::AddEventListener(UObject* listener)
{
	if (listener != nullptr && listener->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
	{
		eventListeners.Add(TWeakObjectPtr<>(listener));
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
