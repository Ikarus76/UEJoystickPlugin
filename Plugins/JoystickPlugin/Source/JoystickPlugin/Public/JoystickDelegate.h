#pragma once

#include "JoystickInterface.h"

//Input Mapping Keys
struct EKeysJoystick
{
	static const int MaxJoystickButtons = 32;
	static const FKey JoystickButton[MaxJoystickButtons];

	static const FKey JoystickAxisX;
	static const FKey JoystickAxisY;
	static const FKey JoystickAxisZ;

	static const FKey JoystickRAxisX;
	static const FKey JoystickRAxisY;
	static const FKey JoystickRAxisZ;

	static const FKey JoystickPOVX[4];
	static const FKey JoystickPOVY[4];

	static const FKey JoystickSlider[2];
};

FVector2D POVAxis(JoystickPOVDirection povValue);

struct JoystickData {
	uint64 buttonsPressedL;
	uint64 buttonsPressedH;
	FVector Axis;
	FVector RAxis;
	JoystickPOVDirection POV[4];
	FVector2D Slider;
};

struct JoystickInfo {
	GUID InstanceId;
	GUID ProductId;
	FName ProductName;
	FName InstanceName;
	bool Connected;
};

class JoystickDelegate
{
	friend class FJoystickPlugin;
public:
	virtual ~JoystickDelegate() {}

	virtual void JoystickButtonPressed(int32 ButtonVal, int32 player);
	virtual void JoystickButtonReleased(int32 ButtonVal, int32 player);
	virtual void AxisChanged(FVector vector, int32 player);
	virtual void RAxisChanged(FVector vector, int32 player);
	virtual void POVChanged(JoystickPOVDirection Value, int32 index, int32 player);
	virtual void SliderChanged(FVector2D Value, int32 player);

	//JoystickHotPlugInterface events
	virtual void JoystickPluggedIn(int32 player);
	virtual void JoystickUnplugged(int32 player);

	virtual bool JoystickIsAvailable();

	//If you want an alternate delegate, set it here
	void SetInterfaceDelegate(UObject* newDelegate);

	//** Required Calls */
	virtual void JoystickStartup();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickStop();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickTick(float DeltaTime);	//Call this every tick

protected:
	UObject* ValidSelfPointer;	//REQUIRED: has to be set before HydraStartup by a UObject subclass

	TArray<JoystickData> LatestFrame;
	TArray<JoystickInfo> Joysticks;

private:
	UObject* _interfaceDelegate;
	bool implementsInterface();
};
