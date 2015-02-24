/*
*
* Copyright (C) <2014> samiljan <Sam Persson>, tsky <thomas.kollakowksy@w-hs.de>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

#include "JoystickPluginPrivatePCH.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

DEFINE_LOG_CATEGORY(JoystickPluginLog);

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

DeviceSDL::DeviceSDL(JoystickEventInterface * eventInterface) :
	hasJoysticks(false),
	hasGameController(false),
	hasHaptic(false),
	eventInterface(eventInterface)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL Starting"));

	resetDevices();

	initSDL();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

DeviceSDL::~DeviceSDL()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL Closing"));
	doneSDL();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////


void DeviceSDL::initSDL()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL()"));

	int result = -1;

	SDL_Init(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init 0"));

	result = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem gamecontroller"));
		hasGameController = true;
	}

	result = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem joystick"));
		hasJoysticks = true;
	}

	result = SDL_InitSubSystem(SDL_INIT_HAPTIC);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem haptic"));
		hasHaptic = true;
	}

	SDL_JoystickEventState(SDL_ENABLE);
}

void DeviceSDL::doneSDL()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::DoneSDL()"));

	for (auto & device : m_Devices) {
		doneDevice(device.Key);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void DeviceSDL::resetDevices()
{
	for (auto & device : m_Devices) {
		resetDevice(device.Key);
	}
}

void DeviceSDL::resetDevice(DeviceId iDevice)
{	
	if (m_Devices.Contains(iDevice)) {
		m_Devices[iDevice].deviceId = iDevice;
		m_Devices[iDevice].gameController = NULL;
		m_Devices[iDevice].haptic = NULL;
		m_Devices[iDevice].hasHaptic = false;
		m_Devices[iDevice].isConnected = false;
		m_Devices[iDevice].isGameController = false;
		m_Devices[iDevice].isJoystick = false;
		m_Devices[iDevice].joystick = NULL;
		m_Devices[iDevice].strName = "unknown";
	}	
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

sDeviceInfoSDL * DeviceSDL::getDevice(DeviceId iDevice)
{
	sDeviceInfoSDL *result = NULL;
	if (m_Devices[iDevice].isConnected) {
		result = &(m_Devices[iDevice]);
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool DeviceSDL::initDevice(DeviceIndex iDevice, sDeviceInfoSDL &deviceInfo)
{
	deviceInfo.deviceIndex = iDevice;

	deviceInfo.joystick = SDL_JoystickOpen(iDevice.value);
	if (deviceInfo.joystick == nullptr)
		return false;
	deviceInfo.instanceId = InstanceId(SDL_JoystickInstanceID(deviceInfo.joystick));

	// DEBUG
	deviceInfo.strName = FString(ANSI_TO_TCHAR(SDL_JoystickName(deviceInfo.joystick)));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *deviceInfo.strName);
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Axis %i"), SDL_JoystickNumAxes(deviceInfo.joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Balls %i"), SDL_JoystickNumBalls(deviceInfo.joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Buttons %i"), SDL_JoystickNumButtons(deviceInfo.joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Hats %i"), SDL_JoystickNumHats(deviceInfo.joystick));

	deviceInfo.isGameController = false;
	deviceInfo.isJoystick = true;
	deviceInfo.isConnected = true;

	if (SDL_IsGameController(iDevice.value)) {
		deviceInfo.gameController = SDL_GameControllerOpen(iDevice.value);

		deviceInfo.strName = FString(ANSI_TO_TCHAR(SDL_GameControllerName(deviceInfo.gameController)));
		UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *deviceInfo.strName);
		
		deviceInfo.isGameController = true;
	}

	
	deviceInfo.haptic = SDL_HapticOpen(iDevice.value);
	if (deviceInfo.haptic != nullptr) {
		UE_LOG(JoystickPluginLog, Log, TEXT("--- Rumble device detected"));
		
		if (SDL_HapticRumbleInit(deviceInfo.haptic) != 0) {
			UE_LOG(JoystickPluginLog, Log, TEXT("--- testing Rumble device:"));
			if (SDL_HapticRumblePlay(deviceInfo.haptic, 0.5, 2000) != 0) {
				UE_LOG(JoystickPluginLog, Log, TEXT("--- play Rumble ...."));
				SDL_Delay(2000);

				deviceInfo.hasHaptic = true;
			} else {
				UE_LOG(JoystickPluginLog, Log, TEXT("--- not successful!"));
				deviceInfo.hasHaptic = false;
			}
		}
	}

	for (auto &existingDevice : m_Devices)
	{
		if (!existingDevice.Value.isConnected && existingDevice.Value.strName == deviceInfo.strName)
		{
			deviceInfo.deviceId = existingDevice.Key;
			m_Devices[deviceInfo.deviceId] = deviceInfo;
			m_DeviceMapping.Add(deviceInfo.instanceId, deviceInfo.deviceId);
			return true;
		}
	}

	deviceInfo.deviceId = DeviceId(m_Devices.Num());
	m_Devices.Add(deviceInfo.deviceId, deviceInfo);
	m_DeviceMapping.Add(deviceInfo.instanceId, deviceInfo.deviceId);

	return true;
}

bool DeviceSDL::doneDevice(DeviceId deviceId)
{
	bool result = false;

	sDeviceInfoSDL &deviceInfo = m_Devices[deviceId];

	if (deviceInfo.haptic != NULL) {
		SDL_HapticClose(deviceInfo.haptic);
		deviceInfo.haptic = NULL;
		deviceInfo.hasHaptic = false;
	}

	if (deviceInfo.joystick != NULL) {
		SDL_JoystickClose(deviceInfo.joystick);
		deviceInfo.joystick = NULL;
		deviceInfo.isJoystick = false;
		deviceInfo.isConnected = false;
		result = true;
	}

	if (deviceInfo.gameController != NULL) {
		SDL_GameControllerClose(deviceInfo.gameController);
		deviceInfo.gameController = NULL;
		deviceInfo.isGameController = false;
		deviceInfo.isConnected = false;
		result = true;
	}

	//resetDevice(deviceInfo.deviceId);

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfDevices()
{
	// gamecontroller are joysticks...
	int numberOfDevices = SDL_NumJoysticks();
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getNumberOfDevices() %i"), numberOfDevices);
	return numberOfDevices;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

FString DeviceSDL::getDeviceName(DeviceId InputDeviceIndex)
{
	FString result = "";
	if (m_Devices.Contains(InputDeviceIndex)) {
		result = m_Devices[InputDeviceIndex].strName;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

Sint32 DeviceSDL::getDeviceInstanceId(DeviceId InputDeviceIndex)
{
	Sint32 result = -1;
	if (m_Devices.Contains(InputDeviceIndex)) {
		result = SDL_JoystickInstanceID(m_Devices[InputDeviceIndex].joystick);
	}
	return result;
}

FString DeviceSDL::getDeviceGUIDtoString(DeviceId InputDeviceIndex)
{
	char buffer[32];
	int8 sizeBuffer = sizeof(buffer);
	FString result = "";

	if (m_Devices.Contains(InputDeviceIndex)) {
		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(m_Devices[InputDeviceIndex].deviceIndex.value);
		SDL_JoystickGetGUIDString(guid, buffer, sizeBuffer);
		result = ANSI_TO_TCHAR(buffer);
	}
	return result;
}

FGuid DeviceSDL::getDeviceGUIDtoGUID(DeviceId InputDeviceIndex)
{
	FGuid result;

	if (m_Devices.Contains(InputDeviceIndex)) {
		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(m_Devices[InputDeviceIndex].deviceIndex.value);
		memcpy(&result, &guid, sizeof(FGuid));
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfAxes(DeviceId InputDeviceIndex)
{
	int32 result = 0;

	if (m_Devices.Contains(InputDeviceIndex)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumAxes(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfButtons(DeviceId InputDeviceIndex)
{
	int32 result = 0;

	if (m_Devices.Contains(InputDeviceIndex)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumButtons(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfHats(DeviceId InputDeviceIndex)
{
	int32 result = 0;
	
	if (m_Devices.Contains(InputDeviceIndex)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumHats(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfBalls(DeviceId InputDeviceIndex)
{
	int32 result = 0;

	if (m_Devices.Contains(InputDeviceIndex)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumBalls(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

JoystickPOVDirection SDL_hatValToDirection(int8 value)
{
	switch (value){
	case SDL_HAT_CENTERED:	return DIRECTION_NONE;
	case SDL_HAT_UP:		return DIRECTION_UP;
	case SDL_HAT_RIGHTUP:	return DIRECTION_UP_RIGHT;
	case SDL_HAT_RIGHT:	return DIRECTION_RIGHT;
	case SDL_HAT_RIGHTDOWN:	return DIRECTION_DOWN_RIGHT;
	case SDL_HAT_DOWN:	return DIRECTION_DOWN;
	case SDL_HAT_LEFTDOWN:	return DIRECTION_DOWN_LEFT;
	case SDL_HAT_LEFT:	return DIRECTION_LEFT;
	case SDL_HAT_LEFTUP:	return DIRECTION_UP_LEFT;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Warning, POV unhandled case. %d"), (int32)value);
		return DIRECTION_NONE;
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool DeviceSDL::getDeviceState(FJoystickState &InputData, FJoystickInfo &JoystickInfo, DeviceId InputDeviceIndex)
{
	bool result = false;

	if (!m_Devices.Contains(InputDeviceIndex)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() ERROR index out of range"));
		return result;
	}
	
	result = true;

	sDeviceInfoSDL device = m_Devices[InputDeviceIndex];

	if (device.isJoystick) {
		SDL_JoystickUpdate();

		InputData.NumberOfBalls = SDL_JoystickNumBalls(device.joystick);
		InputData.NumberOfHats = SDL_JoystickNumHats(device.joystick);
		InputData.NumberOfButtons = SDL_JoystickNumButtons(device.joystick);
		InputData.NumberOfAxis = SDL_JoystickNumAxes(device.joystick);

		// clear TArray add zero change value
		// AXES
		InputData.AxisArray.Empty(0);
		float scaledValue = 0;
		for (int iAxes = 0; iAxes < InputData.NumberOfAxis; iAxes++) {
			InputData.AxisArray.Add(0);
			int rawValue = SDL_JoystickGetAxis(device.joystick, iAxes);
			if (rawValue < 0) {
				scaledValue = (rawValue) / 32768.0;
			}
			else {
				scaledValue = (rawValue) / 32767.0;
			}
			InputData.AxisArray[iAxes] = scaledValue;
		}

		// BUTTONS
		InputData.ButtonsArray.Empty(InputData.NumberOfButtons);
		for (int iButton = 0; iButton < InputData.NumberOfButtons; iButton++) {
			InputData.ButtonsArray.Add(SDL_JoystickGetButton(device.joystick, iButton));
		}

		// HATS
		InputData.HatsArray.Empty(InputData.NumberOfHats);
		InputData.POV.Empty(InputData.NumberOfHats);
		for (int iHat = 0; iHat < InputData.NumberOfHats; iHat++) {
			Uint8 hatState = SDL_JoystickGetHat(device.joystick, iHat);
			InputData.HatsArray.Add(SDL_hatValToDirection(hatState));
			InputData.POV.Add(SDL_hatValToDirection(hatState));
		}
	}
	if (device.isGameController) {
		SDL_GameControllerUpdate();
	}
	
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() %s"), device.strName));

	// this is manipulate while init (remove spaces)
	InputData.DeviceName = JoystickInfo.DeviceName;

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool DeviceSDL::getDeviceSDL(DeviceId iDevice, sDeviceInfoSDL &deviceInfo)
{
	bool result = false;

	if (m_Devices[iDevice].gameController != NULL) {
		deviceInfo = m_Devices[iDevice];
		result = true;
	}
	if (m_Devices[iDevice].joystick != NULL) {
		deviceInfo = m_Devices[iDevice];
		result = true;
	}
	
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void DeviceSDL::update()
{
	// This will spam the log heavily, comment it out for real plugins :)
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::update %f"), deltaTime);

	while (SDL_PollEvent(&m_Event_SDL)) {
		switch (m_Event_SDL.type) {
			case SDL_JOYDEVICEADDED:
			{
				//UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEADDED OR SDL_CONTROLLERDEVICEADDED"));
				DeviceIndex deviceIndex = DeviceIndex(m_Event_SDL.cdevice.which);
				eventInterface->JoystickPluggedIn(deviceIndex);
				break;
			}
			case SDL_JOYDEVICEREMOVED:
			{
				//UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEREMOVED OR SDL_CONTROLLERDEVICEREMOVED"));
				InstanceId instanceId = InstanceId(m_Event_SDL.cdevice.which);
				DeviceId deviceId = m_DeviceMapping[instanceId];
				m_DeviceMapping.Remove(instanceId);
				eventInterface->JoystickUnplugged(deviceId);
				break;
			}
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			{
				DeviceId deviceId = m_DeviceMapping[InstanceId(m_Event_SDL.jbutton.which)];
				eventInterface->JoystickButton(deviceId, m_Event_SDL.jbutton.button, m_Event_SDL.jbutton.state == SDL_PRESSED);
				break;
			}
			case SDL_JOYAXISMOTION:
			{
				DeviceId deviceId = m_DeviceMapping[InstanceId(m_Event_SDL.jaxis.which)];
				eventInterface->JoystickAxis(deviceId, m_Event_SDL.jaxis.axis, m_Event_SDL.jaxis.value / (m_Event_SDL.jaxis.value < 0 ? 32768.0f : 32767.0f));
				break;
			}
			case SDL_JOYHATMOTION:
			{
				DeviceId deviceId = m_DeviceMapping[InstanceId(m_Event_SDL.jhat.which)];
				eventInterface->JoystickHat(deviceId, m_Event_SDL.jhat.hat, SDL_hatValToDirection(m_Event_SDL.jhat.value));
				break;
			}
			case SDL_JOYBALLMOTION:
			{
				DeviceId deviceId = m_DeviceMapping[InstanceId(m_Event_SDL.jball.which)];
				eventInterface->JoystickBall(deviceId, m_Event_SDL.jball.ball, m_Event_SDL.jball.xrel, m_Event_SDL.jball.yrel);
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

