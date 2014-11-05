#pragma once

#include "JoystickDelegate.generated.h"

//#define MAX_CONTROLLERS_SUPPORTED 2


UENUM(BlueprintType)
enum JoystickButtons
{
	JOY_BUTTON_1 = 1,
	JOY_BUTOTN_2 = 2,
	JOY_BUTOTN_3 = 4,
	JOY_BUTOTN_4 = 8,
	JOY_BUTOTN_5 = 16,
	JOY_BUTOTN_6 = 32,
	JOY_BUTOTN_7 = 64,
	JOY_BUTOTN_8 = 128
};


/*
//Input Mapping Keys
struct EKeysHydra
{
	static const FKey HydraLeftJoystickX;
};*/

/** 
 * Converted Controller Data.
 * Contains converted raw and integrated 
 * sixense controller data.
 */
typedef struct _joystickControllerDataUE{
	//raw converted
	int32 buttonsPressed;
	int32 XAxis;
	int32 YAxis;
	int32 ZAxis;
	int32 RXAxis;
	int32 RYAxis;
	int32 RZAxis;
	int32 POV1;
	int32 Slider1;
	bool enabled;
} joystickControllerDataUE;

/*
typedef struct _sixenseAllControllerDataUE{
	sixenseControllerDataUE controllers[4];		//current sdk max supported, untested on STEM or >2 controllers.
	int32 enabledCount;
	bool available;
}sixenseAllControllerDataUE;*/

class JoystickDelegate
{
	friend class FJoystickPlugin;
public:
	//Namespace Hydra for variables, functions and events.

	/** Latest will always contain the freshest controller data, external pointer do not delete*/
	joystickControllerDataUE* _joystickLatestData;

	/** Holds last 10 controller captures, useful for gesture recognition, external pointer do not delete
	joystickControllerDataUE* JoystickHistoryData;	//dynamic array size 10
	*/

	/** Event Emitters, override to receive notifications.
	 *	int32 controller is the controller index (typically 0 or 1 for hydra) 
	 *	Call HydraWhichHand(controller index) to determine which hand is being held (determined and reset on docking)
	 */

	
	virtual void JoystickButtonPressed(int32 ButtonVal);
	virtual void JoystickButtonReleased(int32 ButtonVal);
	virtual void XAxisChanged(int32 Value);
	virtual void YAxisChanged(int32 Value);
	virtual void ZAxisChanged(int32 Value);
	virtual void RXAxisChanged(int32 Value);
	virtual void RYAxisChanged(int32 Value);
	virtual void RZAxisChanged(int32 Value);
	virtual void POV1Changed(int32 Value);
	virtual void Slider1Changed(int32 Value);

	virtual bool JoystickIsAvailable();

	virtual joystickControllerDataUE* JoystickGetLatestData();

	//** Required Calls */
	virtual void JoystickStartup();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickTick(float DeltaTime);	//Call this every tick
};