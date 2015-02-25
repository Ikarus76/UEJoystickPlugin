#include "JoystickPluginPrivatePCH.h"
#include "JoystickDevice.h"

#include "DeviceSDL.h"

#include <SlateBasics.h>
#include <Text.h>

#define LOCTEXT_NAMESPACE "JoystickPlugin"

//Init and Runtime

FJoystickDevice::FJoystickDevice()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::StartupModule() creating Device SDL"));

	DeviceSDL = MakeShareable(new FDeviceSDL(this));

	// Trigger initial SDL_JOYDEVICEADDED events
	DeviceSDL->Update();
}

bool FJoystickDevice::AddInputDevice(FDeviceId DeviceId)
{
	bool result = true; // be optimistic

	if (InputDevices.Contains(DeviceId))
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("already an registered device %s %i"), *InputDevices[DeviceId].DeviceName, DeviceId.value);
		result = false;
	}
	else
	{
		FDeviceInfoSDL * deviceInfoSDL = DeviceSDL->GetDevice(DeviceId);
		if (deviceInfoSDL != nullptr)
		{
			FJoystickInfo deviceInfo;

			deviceInfo.Connected = deviceInfoSDL->bIsConnected;
			deviceInfo.DeviceId = DeviceId.value;
			deviceInfo.Player = 0;

			deviceInfo.ProductId = FDeviceSDL::DeviceGUIDtoGUID(deviceInfoSDL->DeviceIndex);
			deviceInfo.ProductName = deviceInfoSDL->Name;
			deviceInfo.DeviceName = deviceInfo.ProductName.Replace(TEXT(" "), TEXT(""));

			deviceInfo.IsGameController = deviceInfoSDL->GameController != nullptr;

			UE_LOG(JoystickPluginLog, Log, TEXT("add device %s %i"), *deviceInfo.DeviceName, DeviceId.value);
			InputDevices.Add(DeviceId, deviceInfo);

			FJoystickState newDeviceState(DeviceId.value);
			if (DeviceSDL->GetDeviceState(newDeviceState, InputDevices[DeviceId], DeviceId))
			{
				// create FKeyDetails for axis
				DeviceAxisKeys.Add(DeviceId);
				for (int iAxis = 0; iAxis < newDeviceState.NumberOfAxis; iAxis++)
				{
					FString strName = FString::Printf(TEXT("Joystick_%s_Axis%d"), *deviceInfo.DeviceName, iAxis);
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId.value);
					DeviceAxisKeys[DeviceId].Add(FKey(FName(*strName)));

					if (!EKeys::GetKeyDetails(DeviceAxisKeys[DeviceId][iAxis]).IsValid())
					{
						FText textValue = FText::Format(LOCTEXT("DeviceAxis", "{0} Axis {1}"), FText::FromString(deviceInfo.ProductName), FText::AsNumber(iAxis));
						EKeys::AddKey(FKeyDetails(DeviceAxisKeys[DeviceId][iAxis], textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
					}
				}

				// create FKeyDetails for buttons
				DeviceButtonKeys.Add(DeviceId);
				for (int iButton = 0; iButton < newDeviceState.NumberOfButtons; iButton++)
				{
					FString strName = FString::Printf(TEXT("Joystick_%s_Button%d"), *deviceInfo.DeviceName, iButton);
					UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId.value);
					DeviceButtonKeys[DeviceId].Add(FKey(FName(*strName)));

					if (!EKeys::GetKeyDetails(DeviceButtonKeys[DeviceId][iButton]).IsValid())
					{
						FText textValue = FText::Format(LOCTEXT("DeviceButton", "{0} Button {1}"), FText::FromString(deviceInfo.ProductName), FText::AsNumber(iButton));
						EKeys::AddKey(FKeyDetails(DeviceButtonKeys[DeviceId][iButton], textValue, FKeyDetails::GamepadKey));
					}
				}

				FString _2DaxisNames[] = { TEXT("X"), TEXT("Y") };

				// create FKeyDetails for hats
				for (int iAxis = 0; iAxis < 2; iAxis++)
				{
					DeviceHatKeys[iAxis].Add(DeviceId);
					for (int iHat = 0; iHat < newDeviceState.NumberOfHats; iHat++)
					{
						FString strName = FString::Printf(TEXT("Joystick_%s_Hat%d_%s"), *deviceInfo.DeviceName, iHat, *_2DaxisNames[iAxis]);
						UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId.value);
						FKey key{ *strName };
						DeviceHatKeys[iAxis][DeviceId].Add(key);

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
				deviceBallKeys[iAxis].Add(DeviceId);
				for (int iBall = 0; iBall < newDeviceState.NumberOfBalls; iBall++)
				{
				FString strName = FString::Printf(TEXT("Joystick_%s_Ball%d_%s"), *strDeviceName, iBall, _2DaxisNames[iAxis]);
				UE_LOG(JoystickPluginLog, Log, TEXT("add %s %i"), *strName, DeviceId.value);
				FKey key{ *strName };
				deviceBallKeys[iAxis][DeviceId].Add(key);

				if (!EKeys::GetKeyDetails(key).IsValid())
				{
				FText textValue = FText::Format(LOCTEXT("DeviceBall", "{0} Ball {1} {2}"), FText::FromString(strDeviceName), FText::AsNumber(iBall), FText::FromString(_2DaxisNames[iAxis]));
				EKeys::AddKey(FKeyDetails(key, textValue, FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
				}
				}
				}*/

				PreviousState.Add(DeviceId, newDeviceState);
				CurrentState.Add(DeviceId, newDeviceState);
			}
		}
	}
	return result;
}

//Public API Implementation

void FJoystickDevice::JoystickPluggedIn(FDeviceIndex DeviceIndex)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("FJoystickPlugin::JoystickPluggedIn() %i"), DeviceIndex.value);

	FDeviceInfoSDL DeviceInfoSDL;
	if (DeviceSDL->InitDevice(DeviceIndex, DeviceInfoSDL))
	{
		if (DeviceInfoSDL.bIsConnected)
		{
			bool result = AddInputDevice(DeviceInfoSDL.DeviceId);
			if (result)
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("	SUCCESS add device %i"), DeviceInfoSDL.DeviceId.value);
				for (auto & listener : EventListeners)
				{
					UObject * o = listener.Get();
					if (o != nullptr)
					{
						IJoystickInterface::Execute_JoystickPluggedIn(o, DeviceInfoSDL.DeviceId.value);
					}
				}
			}
			else
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("	FAILED add device %i"), DeviceInfoSDL.DeviceId.value);
			}
		}
	}
	else
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("!!WARNING!! no device %i found"), DeviceIndex.value);
	}
}

void FJoystickDevice::JoystickUnplugged(FDeviceId DeviceId)
{
	DeviceSDL->DoneDevice(DeviceId);

	InputDevices[DeviceId].Connected = false;

	//DelegateTick(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("Joystick %d disconnected"), DeviceId.value);

	for (auto & listener : EventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickUnplugged(o, DeviceId.value);
		}
	}
}

bool EmitKeyUpEventForKey(FKey Key, int32 User, bool Repeat)
{
	FKeyEvent KeyEvent(Key, FSlateApplication::Get().GetModifierKeys(), User, Repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
}

bool EmitKeyDownEventForKey(FKey Key, int32 User, bool Repeat)
{
	FKeyEvent KeyEvent(Key, FSlateApplication::Get().GetModifierKeys(), User, Repeat, 0, 0);
	return FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);
}

void FJoystickDevice::JoystickButton(FDeviceId DeviceId, int32 Button, bool Pressed)
{
	CurrentState[DeviceId].ButtonsArray[Button] = Pressed;
	if (Pressed)
		EmitKeyDownEventForKey(DeviceButtonKeys[DeviceId][Button], InputDevices[DeviceId].Player, false);
	else
		EmitKeyUpEventForKey(DeviceButtonKeys[DeviceId][Button], InputDevices[DeviceId].Player, false);

	for (auto & listener : EventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			if (Pressed)
				IJoystickInterface::Execute_JoystickButtonPressed(o, Button, CurrentState[DeviceId]);
			else
				IJoystickInterface::Execute_JoystickButtonReleased(o, Button, CurrentState[DeviceId]);
		}
	}
}

bool EmitAnalogInputEventForKey(FKey Key, float Value, int32 User, bool Repeat)
{
	FAnalogInputEvent AnalogInputEvent(Key, FSlateApplication::Get().GetModifierKeys(), User, Repeat, 0, 0, Value);
	return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
}

void FJoystickDevice::JoystickAxis(FDeviceId DeviceId, int32 Axis, float Value)
{
	CurrentState[DeviceId].AxisArray[Axis] = Value;
	EmitAnalogInputEventForKey(DeviceAxisKeys[DeviceId][Axis], Value, InputDevices[DeviceId].Player, 0);

	for (auto & listener : EventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickAxisArrayChanged(o, Axis, CurrentState[DeviceId].AxisArray[Axis], PreviousState[DeviceId].AxisArray[Axis], CurrentState[DeviceId], PreviousState[DeviceId]);
		}
	}
}

void FJoystickDevice::JoystickHat(FDeviceId DeviceId, int32 Hat, EJoystickPOVDirection Value)
{
	CurrentState[DeviceId].HatsArray[Hat] = Value;

	FVector2D povAxis = POVAxis(Value);
	EmitAnalogInputEventForKey(DeviceHatKeys[0][DeviceId][Hat], povAxis.X, InputDevices[DeviceId].Player, 0);
	EmitAnalogInputEventForKey(DeviceHatKeys[1][DeviceId][Hat], povAxis.Y, InputDevices[DeviceId].Player, 0);

	for (auto & listener : EventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickHatsArrayChanged(o, Hat, Value, CurrentState[DeviceId]);
		}
	}
}

bool EmitPointerEventForKey(int32 PointerIndex, const FVector2D &Value)
{
	FPointerEvent pointerEvent(PointerIndex, FVector2D::ZeroVector, FVector2D::ZeroVector, Value, TSet<FKey>(), FSlateApplication::Get().GetModifierKeys());
	return FSlateApplication::Get().ProcessMouseMoveEvent(pointerEvent);
}

void FJoystickDevice::JoystickBall(FDeviceId DeviceId, int32 Ball, int DeltaX, int DeltaY)
{
	// EmitPointerEventForKey(ball, FVector2D(dx, dy)); TODO: Test how this works with an actual "ball"

	for (auto & listener : EventListeners)
	{
		UObject * o = listener.Get();
		if (o != nullptr)
		{
			IJoystickInterface::Execute_JoystickBallsArrayChanged(o, Ball, DeltaX, DeltaY, CurrentState[DeviceId]);
		}
	}
}

// IInputDevice implementation

void FJoystickDevice::Tick(float DeltaTime)
{

}

void FJoystickDevice::SendControllerEvents()
{
	for (auto & Device : InputDevices) 
	{
		if (InputDevices.Contains(Device.Key)) 
		{
			if (InputDevices[Device.Key].Connected) 
			{
				PreviousState[Device.Key] = CurrentState[Device.Key];
			}
		}
	}

	DeviceSDL->Update();

	// Clean up weak references
	for (int i = 0; i < EventListeners.Num(); i++)
	{
		if (!EventListeners[i].IsValid())
		{
			EventListeners.RemoveAt(i);
			i--;
		}
	}
}

void FJoystickDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
}

bool FJoystickDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FJoystickDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
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

void FJoystickDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values)
{
	//FControllerState& ControllerState = ControllerStates[ControllerId];
	//if (ControllerState.bIsConnected)
	//{
	//	ControllerState.ForceFeedback = Values;
	//}
}

bool FJoystickDevice::AddEventListener(UObject* Listener)
{
	if (Listener != nullptr && Listener->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
	{
		EventListeners.Add(TWeakObjectPtr<>(Listener));
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
