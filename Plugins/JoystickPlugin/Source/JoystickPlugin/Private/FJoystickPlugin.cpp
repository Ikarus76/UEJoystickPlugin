#include "JoystickPluginPrivatePCH.h"

#include <SlateBasics.h>

#include "IJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "FJoystickPlugin.h"

#include "WinJoystick.h"

IMPLEMENT_MODULE(FJoystickPlugin, JoystickPlugin)

#define LOCTEXT_NAMESPACE "JoystickPlugin"

//Init and Runtime
void FJoystickPlugin::StartupModule()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("Attempting to startup Joystick Module."));

	//Add the keys either way, these should always be available when plugin starts up

	for (int i = 0; i < EKeysJoystick::MaxJoystickButtons; i++)
	{
		EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton[i], FText::Format(LOCTEXT("JoystickButton", "Joystick Button {0}"), FText::AsNumber(i + 1)), FKeyDetails::GamepadKey));
	}

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisX, LOCTEXT("JoystickAxisX", "Joystick Axis X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisY, LOCTEXT("JoystickAxisY", "Joystick Axis Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisZ, LOCTEXT("JoystickAxisZ", "Joystick Axis Z"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisX, LOCTEXT("JoystickRAxisX", "Joystick RAxis X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisY, LOCTEXT("JoystickRAxisY", "Joystick RAxis Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisZ, LOCTEXT("JoystickRAxisZ", "Joystick RAxis Z"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	for (int i = 0; i < 4; i++)
	{
		EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOVX[i], FText::Format(LOCTEXT("JoystickPOVX", "Joystick POV{0} X"), FText::AsNumber(i + 1)), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
		EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOVY[i], FText::Format(LOCTEXT("JoystickPOVY", "Joystick POV{0} Y"), FText::AsNumber(i + 1)), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	}

	for (int i = 0; i < 2; i++)
	{
		EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickSlider[i], FText::Format(LOCTEXT("JoystickSlider", "Joystick Slider {0}"), FText::AsNumber(i + 1)), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	}

	hpDelegate = this;

	//Add our hotplugging listener
	EnableHotPlugListener();

	if (S_OK == InitDirectInput()){
		UE_LOG(JoystickPluginLog, Log, TEXT("Direct Input initialized."));
	}
	else{
		UE_LOG(JoystickPluginLog, Log, TEXT("Direct Input initialization failed."));
	}
}


void FJoystickPlugin::ShutdownModule()
{
	//Cleanup forcefeedback
	if (g_pJoystickFF)
		CleanupFF();

	CleanupHotPlugging();
}


//Public API Implementation

/** Public API - Required **/

void FJoystickPlugin::SetDelegate(JoystickDelegate* newDelegate)
{
	joystickDelegate = newDelegate;
	if (joystickDelegate)
	{
		//Start case, if we started the plugin with a joystick already plugged in
		joystickDelegate->Joysticks = joysticks;
		for (auto &joystick : joysticks)
			joystickDelegate->JoystickPluggedIn(joystick);
	}
}

void FJoystickPlugin::JoystickPluggedIn(FJoystickInfo & joystick)
{
	// First try to find the same joystick if it was connected before (to get the same index)
	joystick.Player = joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.InstanceId == joystick.InstanceId; });
	if (joystick.Player == INDEX_NONE)
	{
		// Otherwise try to find an unused slot
		joystick.Player = joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.Connected == false; });
	}

	if (joystick.Player == INDEX_NONE)
	{
		// Finally add a now slot
		joystick.Player = joysticks.Add(joystick);
		joysticks[joystick.Player].Player = joystick.Player;
		prevData.Add(FJoystickState(joystick.Player));
		currData.Add(FJoystickState(joystick.Player));
	}
	else
	{
		joysticks[joystick.Player] = joystick;
	}

	if (joystickDelegate)
	{
		joystickDelegate->Joysticks = joysticks;
		joystickDelegate->JoystickPluggedIn(joystick);
	}
}

void FJoystickPlugin::JoystickUnplugged(FGuid id)
{
	int player = joysticks.IndexOfByPredicate([&](const FJoystickInfo &j) { return j.InstanceId == id; });
	if (player == INDEX_NONE)
	{
		// Can happen e.g. if we fail to acquire a joystick
		return;
	}

	joysticks[player].Connected = false;

	DelegateTick(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("Joystick for player %d disconnected"), player);

	if (joystickDelegate)
	{
		// Let joystick state return to zero
		joystickDelegate->JoystickUnplugged(joysticks[player]);
	}
}

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
	for (int i = 0; i < joysticks.Num(); i++)
	{
		if (!joysticks[i].Connected)
			continue;

		//get the freshest data
		FJoystickState newJoyData(i);
		if (GetDeviceState(newJoyData, ToGUID(joysticks[i].InstanceId)))
		{
			prevData[i] = currData[i];
			currData[i] = newJoyData;
		}
	}

	DelegateTick(DeltaTime);
}

//UE v4.6 IM event wrappers
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

bool EmitAnalogInputEventForKey(FKey key, float value, int32 user, bool repeat)
{
	FAnalogInputEvent AnalogInputEvent(key, FSlateApplication::Get().GetModifierKeys(), user, repeat, 0, 0, value);
	return FSlateApplication::Get().ProcessAnalogInputEvent(AnalogInputEvent);
}

/** Internal Tick - Called by the Plugin */
void FJoystickPlugin::DelegateTick(float DeltaTime)
{
	//Update delegate
	if (joystickDelegate)
		joystickDelegate->LatestFrame = currData;

	for (int32 p = 0; p < joysticks.Num(); p++)
	{
		auto const & current = currData[p];
		auto const & prev = prevData[p];

		// check buttons 
		for (uint64 i = 0; i < 128; i++)
		{
			uint64 currBitmask = i < 64 ? current.buttonsPressedL : current.buttonsPressedH;
			uint64 prevBitmask = i < 64 ? prev.buttonsPressedL : prev.buttonsPressedH;

			int index = i < 64 ? i : i - 64;
			uint64 bitVal = uint64(1) << index;

			if ((currBitmask & bitVal) != (prevBitmask & bitVal))
			{
				// button state has changed
				if (currBitmask & bitVal)
				{
					if (joystickDelegate)
						joystickDelegate->JoystickButtonPressed(i + 1, current);
					if (i < EKeysJoystick::MaxJoystickButtons)
						EmitKeyDownEventForKey(EKeysJoystick::JoystickButton[i], p, 0);
				}
				else
				{
					if (joystickDelegate)
						joystickDelegate->JoystickButtonReleased(i + 1, current);
					if (i < EKeysJoystick::MaxJoystickButtons)
						EmitKeyUpEventForKey(EKeysJoystick::JoystickButton[i], p, 0);
				}
			}
		}

		//check axis
		if (current.Axis != prev.Axis)
		{
			if (joystickDelegate)
				joystickDelegate->AxisChanged(current.Axis, current);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickAxisX, current.Axis.X, p, 0);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickAxisY, current.Axis.Y, p, 0);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickAxisZ, current.Axis.Z, p, 0);
		}

		//check rotation axis
		if (current.RAxis != prev.RAxis)
		{
			if (joystickDelegate)
				joystickDelegate->RAxisChanged(current.RAxis, current);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickRAxisX, current.RAxis.X, p, 0);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickRAxisY, current.RAxis.Y, p, 0);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickRAxisZ, current.RAxis.Z, p, 0);
		}

		for (int i = 0; i < 4; i++)
		{
			if (current.POV[i] != prev.POV[i])
			{
				if (joystickDelegate)
					joystickDelegate->POVChanged(current.POV[i], i, current);
				FVector2D direction = POVAxis(current.POV[i]);
				EmitAnalogInputEventForKey(EKeysJoystick::JoystickPOVX[i], direction.X, p, 0);
				EmitAnalogInputEventForKey(EKeysJoystick::JoystickPOVY[i], direction.Y, p, 0);
			}
		}

		//check slider
		if (current.Slider != prev.Slider)
		{
			if (joystickDelegate)
				joystickDelegate->SliderChanged(current.Slider, current);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickSlider[0], current.Slider.X, p, 0);
			EmitAnalogInputEventForKey(EKeysJoystick::JoystickSlider[1], current.Slider.Y, p, 0);
		}
	}
}

void FJoystickPlugin::ForceFeedbackXY(int32 x, int32 y, float magnitudeScale)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("Force feedback currently not implemented correctly, aborting."));
	return;

	if (!g_pJoystickFF){
		UE_LOG(JoystickPluginLog, Log, TEXT("Force feedback not available."));
		return;
	}

	//scale the input to joystick scaling
	SetForceFeedbackXY(x, y, magnitudeScale);
}


#undef LOCTEXT_NAMESPACE
