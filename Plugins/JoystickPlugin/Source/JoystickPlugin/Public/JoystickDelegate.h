#pragma once

#include "JoystickInterface.h"

/*UENUM(BlueprintType)
enum JoystickButtons
{
	JOY_BUTTON_1 = 1,
	JOY_BUTTON_2 = 2,
	JOY_BUTTON_3 = 4,
	JOY_BUTTON_4 = 8,
	JOY_BUTTON_5 = 16,
	JOY_BUTTON_6 = 32,
	JOY_BUTTON_7 = 64,
	JOY_BUTTON_8 = 128
};*/




//Input Mapping Keys
struct EKeysJoystick
{
	//For now input mapping supports 16 buttons on the joystick, but DirectInput supports potentially up to 128 buttons
	static const FKey JoystickButton1;
	static const FKey JoystickButton2;
	static const FKey JoystickButton3;
	static const FKey JoystickButton4;
	static const FKey JoystickButton5;
	static const FKey JoystickButton6;
	static const FKey JoystickButton7;
	static const FKey JoystickButton8;
	static const FKey JoystickButton9;
	static const FKey JoystickButton10;
	static const FKey JoystickButton11;
	static const FKey JoystickButton12;
	static const FKey JoystickButton13;
	static const FKey JoystickButton14;
	static const FKey JoystickButton15;
	static const FKey JoystickButton16;

	static const FKey JoystickAxisX;
	static const FKey JoystickAxisY;
	static const FKey JoystickAxisZ;

	static const FKey JoystickRAxisX;
	static const FKey JoystickRAxisY;
	static const FKey JoystickRAxisZ;

	static const FKey JoystickPOVX;
	static const FKey JoystickPOVY;
	static const FKey JoystickSlider;
};

struct  joystickControllerDataUE{
	//raw converted
	int32 buttonsPressed;
	FVector Axis;
	FVector RAxis;
	int32 POV1;
	int32 Slider1;
	bool enabled;
};

class JoystickDelegate
{
	friend class FJoystickPlugin;
public:
	
	virtual void JoystickButtonPressed(int32 ButtonVal, class UJoystickSingleController* frame);
	virtual void JoystickButtonReleased(int32 ButtonVal, class UJoystickSingleController* frame);
	virtual void AxisChanged(FVector vector, class UJoystickSingleController* frame);
	virtual void RAxisChanged(FVector vector, class UJoystickSingleController* frame);
	virtual void POVChanged(JoystickPOVDirection Value, class UJoystickSingleController* frame);
	virtual void SliderChanged(float Value, class UJoystickSingleController* frame);

	virtual bool JoystickIsAvailable();

	//** Get the latest available data given in a single frame. Valid ControllerId is 0 or 1  */
	class UJoystickSingleController* JoystickGetLatestFrame();

	//If you want an alternate delegate, set it here
	void SetInterfaceDelegate(UObject* newDelegate);

	//** Required Calls */
	virtual void JoystickStartup();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickTick(float DeltaTime);	//Call this every tick
		
protected:
	UObject* ValidSelfPointer;	//REQUIRED: has to be set before HydraStartup by a UObject subclass
	class UJoystickSingleController* _latestFrame;

private:
	UObject* _interfaceDelegate;
	bool implementsInterface();
};