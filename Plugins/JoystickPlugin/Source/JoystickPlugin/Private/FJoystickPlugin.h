#pragma once

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class DataCollector;
class JoystickDelegate;
class DeviceSDL;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class JoystickHotPlugInterface
{
public:
	virtual ~JoystickHotPlugInterface()
	{
	}

	virtual void JoystickPluggedIn(int32 iDevice) = 0;
	virtual void JoystickUnplugged(int32 iDevice) = 0;
};

class FJoystickPlugin : public IJoystickPlugin, public JoystickHotPlugInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;

	/** To subscribe to event calls, only supports one listener for now */
	void SetDelegate(JoystickDelegate* newDelegate) override;

	/** Call this in your class Tick to update information */
	void JoystickTick(float DeltaTime) override;

	void ForceFeedbackXY(int32 x, int32 y, float magnitudeScale) override;

	bool JoystickIsAvailable() override;
	
	void JoystickPluggedIn(int32 iDevice) override;
	void JoystickUnplugged(int32 iDevice) override;

	bool AddInputDevice(int iDevice);
	bool RemoveInputDevice(int iDevice);

private:

	DeviceSDL *m_DeviceSDL;

	DataCollector *m_pCollector;
	JoystickDelegate* joystickDelegate = nullptr;
	void* DLLHandle;

	TArray<FJoystickState> currData;
	TArray<FJoystickState> prevData;

	TArray<FJoystickInfo> m_InputDevices;

	//Delegate Private functions
	void DelegateTick(float DeltaTime);

};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
