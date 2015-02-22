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

class JoystickEventInterface
{
public:
	virtual ~JoystickEventInterface()
	{
	}

	virtual void JoystickPluggedIn(DeviceIndex iDevice) = 0;
	virtual void JoystickUnplugged(DeviceId iDevice) = 0;
	virtual void JoystickButton(DeviceId iDevice, int32 button, bool pressed) = 0;
	virtual void JoystickAxis(DeviceId iDevice, int32 axis, float value) = 0;
	virtual void JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value) = 0;
	virtual void JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy) = 0;
};

class FJoystickPlugin : public IJoystickPlugin, public JoystickEventInterface
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
	
	void JoystickPluggedIn(DeviceIndex iDevice) override;
	void JoystickUnplugged(DeviceId iDevice) override;
	void JoystickButton(DeviceId iDevice, int32 button, bool pressed) override;
	void JoystickAxis(DeviceId iDevice, int32 axis, float value) override;
	void JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value) override;
	void JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy) override;

	bool AddInputDevice(DeviceId iDevice);
	bool RemoveInputDevice(DeviceId iDevice);

private:

	DeviceSDL *m_DeviceSDL = nullptr;
	JoystickDelegate* joystickDelegate = nullptr;

	TMap<DeviceId, FJoystickState> currData;
	TMap<DeviceId, FJoystickState> prevData;

	TMap<DeviceId, FJoystickInfo> m_InputDevices;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
