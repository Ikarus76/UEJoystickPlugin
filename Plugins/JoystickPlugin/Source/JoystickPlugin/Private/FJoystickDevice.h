#pragma once

#include "Runtime/InputDevice/Public/IInputDevice.h"

namespace { // Avoid conflict with other Unreal Engine classes

	static const int32 MAX_NUM_CONTROLLER_BUTTONS = 4;

	typedef void *IOJoystickDevicePtr;
	typedef void *IOJoystickManagerPtr;

	struct FJoystickButtonInfo {
		uint8 Value;
	};

	struct FJoystickAxisInfo {
		int32 xValue = 0;
		int32 yValue = 0;
		int32 MinValue = 0;
		int32 MaxValue = 65535;
	};

	struct FJoystickDeviceInfo {
		IOJoystickDevicePtr DevicePtr = nullptr;
		TArray<FJoystickButtonInfo> Buttons;
		FJoystickAxisInfo AxisInfo;
	};

	struct FControllerState {
		/** Last frame's button states, so we only send events on edges */
		uint8 ButtonStates[MAX_NUM_CONTROLLER_BUTTONS];

		/** Next time a repeat event should be generated for each button */
		double NextRepeatTime[MAX_NUM_CONTROLLER_BUTTONS];

		/** Raw x analog value */
		int16 xAnalog;

		/** Raw y analog value */
		int16 yAnalog;

		/** Id of the controller */
		int32 ControllerId;

		FJoystickDeviceInfo Device;
	};
}


/*
**
* Joystick input interface.
*
class FJoystickDevice : public IInputDevice
{
public:
	explicit FJoystickDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	virtual ~FJoystickDevice();

	/** Tick the interface (e.g. check for new controllers) 
	virtual void Tick(float DeltaTime) OVERRIDE;

	/** Poll for controller state and send events if needed 
	virtual void SendControllerEvents() OVERRIDE;

	/** Set which MessageHandler will get the events from SendControllerEvents. 
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) OVERRIDE;

	/** Exec handler to allow console commands to be passed through for debugging 
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) OVERRIDE;

	
	* IForceFeedbackSystem pass through functions
	
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) OVERRIDE;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) OVERRIDE;

private:
	/** Names of all the buttons 
	EControllerButtons::Type Buttons[MAX_NUM_CONTROLLER_BUTTONS];

	/** Controller state 
	FControllerState ControllerState;

	/** Delay before sending a repeat message after a button was first pressed 
	float InitialButtonRepeatDelay;

	/** Delay before sending a repeat message after a button has been pressed for a while 
	float ButtonRepeatDelay;

	IOJoystickManagerPtr JoystickManager;

	TSharedRef< FGenericApplicationMessageHandler > MessageHandler;
};
*/