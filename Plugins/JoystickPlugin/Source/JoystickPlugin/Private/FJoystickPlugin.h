#pragma once

class DataCollector;
class JoystickDelegate;

class JoystickHotPlugInterface
{
public:
	virtual void JoystickPluggedIn(){};
	virtual void JoystickUnplugged(){};
};

class FJoystickPlugin : public IJoystickPlugin, public JoystickHotPlugInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();

	/** To subscribe to event calls, only supports one listener for now */
	void SetDelegate(JoystickDelegate* newDelegate);

	/** Call this in your class Tick to update information */
	void JoystickTick(float DeltaTime);

	void ForceFeedbackXY(int32 x, int32 y, float magnitudeScale);

private:
	DataCollector *m_pCollector;
	JoystickDelegate* joystickDelegate;
	void* DLLHandle;

	//Delegate Private functions
	void DelegateTick(float DeltaTime);
	void DelegateUpdateAllData();

	void JoystickPluggedIn() override;
	void JoystickUnplugged() override;
};