#pragma once

#include <Runtime/InputDevice/Public/InputDevice.h>
#include <JoystickInterface.h>

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class DeviceSDL;

struct DeviceIndex
{
	int32 value = -1;
	explicit DeviceIndex(int32 v) : value(v) {}
	bool operator==(const DeviceIndex other) const { return value == other.value; };
};

struct InstanceId
{
	int32 value = -1;
	explicit InstanceId(int32 v) : value(v) {}
	bool operator==(InstanceId other) const { return value == other.value; };
};
FORCEINLINE uint32 GetTypeHash(InstanceId instanceId)
{
	return GetTypeHash(instanceId.value);
}

struct DeviceId
{
	int32 value = -1;
	explicit DeviceId(int32 v) : value(v) {}
	bool operator==(DeviceId other) const { return value == other.value; };
};
FORCEINLINE uint32 GetTypeHash(DeviceId deviceId)
{
	return GetTypeHash(deviceId.value);
}

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

class JoystickDevice : public IInputDevice, public JoystickEventInterface
{
public:
	JoystickDevice();
	~JoystickDevice();

	void Tick(float DeltaTime) override;
	void SendControllerEvents() override;
	void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;
	bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& values) override;
	
	bool AddEventListener(UObject* listener);

	void JoystickPluggedIn(DeviceIndex iDevice) override;
	void JoystickUnplugged(DeviceId iDevice) override;
	void JoystickButton(DeviceId iDevice, int32 button, bool pressed) override;
	void JoystickAxis(DeviceId iDevice, int32 axis, float value) override;
	void JoystickHat(DeviceId iDevice, int32 hat, JoystickPOVDirection value) override;
	void JoystickBall(DeviceId iDevice, int32 ball, int dx, int dy) override;

	TMap<DeviceId, FJoystickState> currData;
	TMap<DeviceId, FJoystickState> prevData;

	TMap<DeviceId, FJoystickInfo> m_InputDevices;
private:
	bool AddInputDevice(DeviceId iDevice);

	DeviceSDL *m_DeviceSDL = nullptr;
	TArray<TWeakObjectPtr<UObject>> eventListeners;

	TMap<DeviceId, TArray<FKey>> deviceButtonKeys;
	TMap<DeviceId, TArray<FKey>> deviceAxisKeys;
	TMap<DeviceId, TArray<FKey>> deviceHatKeys[2];
	//TMap<DeviceId, TArray<FKey>> deviceBallKeys[2];
};

class FJoystickPlugin : public IJoystickPlugin
{
public:
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override
	{
		return JoystickDevice;
	}

	void ShutdownModule() override
	{
		IJoystickPlugin::ShutdownModule();
		JoystickDevice = nullptr;
	}

	void StartupModule() override
	{
		IJoystickPlugin::StartupModule();
		JoystickDevice = MakeShareable(new ::JoystickDevice());
	}

	TSharedPtr< class JoystickDevice > JoystickDevice;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
