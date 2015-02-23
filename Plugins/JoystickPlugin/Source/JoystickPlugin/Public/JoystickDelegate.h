#pragma once

#include "JoystickInterface.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

enum class DeviceIndex : int32 { _min = 0, _invalid = -1 };

enum class InstanceId : int32 { _min = 0, _invalid = -1 };
FORCEINLINE uint32 GetTypeHash(InstanceId instanceId)
{
	return GetTypeHash((int32)instanceId);
}

enum class DeviceId : int32 { _min = 0, _invalid = -1 };
FORCEINLINE uint32 GetTypeHash(DeviceId deviceId)
{
	return GetTypeHash((int32)deviceId);
}


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class JoystickDelegate
{
	friend class FJoystickPlugin;
public:
	virtual ~JoystickDelegate() {}

	virtual void JoystickButtonPressed(int32 ButtonVal, const FJoystickState &state);
	virtual void JoystickButtonReleased(int32 ButtonVal, const FJoystickState &state);
	//virtual void AxisChanged(FVector vector, const FJoystickState &state);

	virtual void AxisArrayChanged(int32 index, float value, float valuePrev, const FJoystickState &state, const FJoystickState &prev);
	virtual void ButtonsArrayChanged(int32 index, bool value, const FJoystickState &state);
	virtual void HatsArrayChanged(int32 index, float value, const FJoystickState &state);
	virtual void BallsArrayChanged(int32 index, int32 dx, int32 dy, const FJoystickState &state);

	//virtual void RAxisChanged(FVector vector, const FJoystickState &state);
	//virtual void POVChanged(JoystickPOVDirection Value, int32 index, const FJoystickState &state);
	//virtual void SliderChanged(FVector2D Value, const FJoystickState &state);

	//JoystickHotPlugInterface events
	virtual void JoystickPluggedIn(int32 iDevice);
	virtual void JoystickUnplugged(int32 iDevice);

	virtual bool JoystickIsAvailable();

	//If you want an alternate delegate, set it here
	void SetInterfaceDelegate(UObject* newDelegate);

	//** Required Calls */
	virtual void JoystickStartup();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickStop();				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickTick(float DeltaTime);	//Call this every tick

protected:
	UObject* ValidSelfPointer;	//REQUIRED: has to be set before HydraStartup by a UObject subclass

	TArray<FJoystickState> PreviousFrame; // @TODO this seems not to work as i wish
	TArray<FJoystickState> LatestFrame;
	TArray<FJoystickInfo> Joysticks;

private:
	UObject* _interfaceDelegate;
	bool implementsInterface();
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
