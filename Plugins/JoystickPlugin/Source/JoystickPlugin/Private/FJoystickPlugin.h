#pragma once

class DataCollector;
class JoystickDelegate;

class FJoystickPlugin : public IJoystickPlugin
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();

	/** To subscribe to event calls, only supports one listener for now */
	void SetDelegate(JoystickDelegate* newDelegate);

	/** Call this in your class Tick to update information */
	void JoystickTick(float DeltaTime);

private:
	DataCollector *m_pCollector;
	JoystickDelegate* joystickDelegate;
	void* DLLHandle;

	//Delegate Private functions
	void DelegateTick(float DeltaTime);
	void DelegateUpdateAllData();
	void DelegateCheckEnabledCount(bool* plugNotChecked);
};