#pragma once

class DataCollector;
class JoystickDelegate;

class JoystickHotPlugInterface
{
public:
	virtual ~JoystickHotPlugInterface()
	{
	}

	virtual void JoystickPluggedIn(const JoystickInfo &joystick) = 0;
	virtual void JoystickUnplugged(GUID id) = 0;
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

private:
	DataCollector *m_pCollector;
	JoystickDelegate* joystickDelegate = nullptr;
	void* DLLHandle;

	TArray<JoystickData> currData;
	TArray<JoystickData> prevData;

	TArray<JoystickInfo> joysticks;

	//Delegate Private functions
	void DelegateTick(float DeltaTime);

	void JoystickPluggedIn(const JoystickInfo &joystick) override;
	void JoystickUnplugged(GUID id) override;
};
