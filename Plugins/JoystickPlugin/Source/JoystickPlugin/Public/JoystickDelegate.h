#pragma once

#include "JoystickInterface.h"

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
	static const FKey JoystickButton17;
	static const FKey JoystickButton18;
	static const FKey JoystickButton19;
	static const FKey JoystickButton20;
	static const FKey JoystickButton21;
	static const FKey JoystickButton22;
	static const FKey JoystickButton23;
	static const FKey JoystickButton24;
	static const FKey JoystickButton25;
	static const FKey JoystickButton26;
	static const FKey JoystickButton27;
	static const FKey JoystickButton28;
	static const FKey JoystickButton29;
	static const FKey JoystickButton30;
	static const FKey JoystickButton31;
	static const FKey JoystickButton32;

	static const FKey JoystickAxisX;
	static const FKey JoystickAxisY;
	static const FKey JoystickAxisZ;

	static const FKey JoystickRAxisX;
	static const FKey JoystickRAxisY;
	static const FKey JoystickRAxisZ;

	static const FKey JoystickPOV1X;
	static const FKey JoystickPOV1Y;

	static const FKey JoystickPOV2X;
	static const FKey JoystickPOV2Y;

	static const FKey JoystickPOV3X;
	static const FKey JoystickPOV3Y;

	static const FKey JoystickSlider1;
	static const FKey JoystickSlider2;
};

struct  joystickControllerDataUE{
	//raw converted
	int64 buttonsPressedL;
	int64 buttonsPressedH;
	FVector Axis;
	FVector RAxis;
	FVector POV = FVector(4294967296, 4294967296, 4294967296);
	FVector2D Slider;
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
	virtual void SliderChanged(FVector2D Value, class UJoystickSingleController* frame);

	//JoystickHotPlugInterface events
	virtual void JoystickPluggedIn();
	virtual void JoystickUnplugged();

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