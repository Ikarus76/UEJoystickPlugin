/*
*
* Copyright (C) <2014> <w-hs - cglab projects>
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

JoystickHotPlugInterface* g_HotPlugDelegate;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

DeviceSDL::DeviceSDL() :
	hasGameController(false),
	hasJoysticks(false),
	hasHaptic(false)
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


	initDevices();
	
}

void DeviceSDL::doneSDL()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::DoneSDL()"));

	for (int iDevice = 0; iDevice < m_NumberOfDevices; iDevice++) {
		doneDevice(m_Devices[iDevice]);
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void DeviceSDL::resetDevices()
{
	for (int iDevice = 0; iDevice < MAX_CONTROLLED_DEVICES; iDevice++) {
		resetDevice(iDevice);
	}
}

void DeviceSDL::resetDevice(int iDevice)
{	
	if (iDevice < MAX_CONTROLLED_DEVICES) {
		m_Devices[iDevice].deviceNumber = iDevice;
		m_Devices[iDevice].gameController = NULL;
		m_Devices[iDevice].haptic = NULL;
		m_Devices[iDevice].hasHaptic = false;
		m_Devices[iDevice].isConnected = false;
		m_Devices[iDevice].isGameController = false;
		m_Devices[iDevice].isJoystick = false;
		m_Devices[iDevice].joystick = NULL;
		m_Devices[iDevice].strName = FString(ANSI_TO_TCHAR("unknown"));
	}	
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool DeviceSDL::initDevices()
{
	bool result = false;

	m_NumberOfDevices = SDL_NumJoysticks();

	UE_LOG(JoystickPluginLog, Log, TEXT("%i devices were found."), m_NumberOfDevices);
	UE_LOG(JoystickPluginLog, Log, TEXT("The names of the devices are:"));

	if (m_NumberOfDevices >= MAX_CONTROLLED_DEVICES) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() !!!WARNING!!! more devices than i can control!!!"));
		m_NumberOfDevices = MAX_CONTROLLED_DEVICES - 1;
	}

	for (int iDevice = 0; iDevice < m_NumberOfDevices; iDevice++) {
		result = initDevice(iDevice, m_Devices[iDevice]);
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

sDeviceInfoSDL * DeviceSDL::getDevice(int iDevice)
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

bool DeviceSDL::initDevice(int iDevice, sDeviceInfoSDL &deviceInfo)
{
	bool result = false;

	if (iDevice > getNumberOfDevices()) {
		return false;
	}
	
	deviceInfo.deviceNumber = iDevice;

	deviceInfo.joystick = SDL_JoystickOpen(iDevice);
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

	result = true;

	if (SDL_IsGameController(iDevice)) {
		deviceInfo.gameController = SDL_GameControllerOpen(iDevice);

		deviceInfo.strName = FString(ANSI_TO_TCHAR(SDL_GameControllerName(deviceInfo.gameController)));
		UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *deviceInfo.strName);
		
		deviceInfo.isGameController = true;
	}

	
	deviceInfo.haptic = SDL_HapticOpen(iDevice);
	if (deviceInfo.haptic != NULL) {
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

	memcpy(&m_Devices[iDevice], &deviceInfo, sizeof(sDeviceInfoSDL));

	return result;
}

bool DeviceSDL::doneDevice(sDeviceInfoSDL &deviceInfo)
{
	bool result = false;

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

	resetDevice(deviceInfo.deviceNumber);

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfDevices()
{
	// gamecontroller are joysticks...
	m_NumberOfDevices = SDL_NumJoysticks();
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getNumberOfDevices() %i"), m_NumberOfDevices);
	return m_NumberOfDevices;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

FString DeviceSDL::getDeviceName(int32 InputDeviceIndex)
{
	FString result = "";
	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		result = m_Devices[InputDeviceIndex].strName;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

Sint32 DeviceSDL::getDeviceInstanceId(int32 InputDeviceIndex)
{
	Sint32 result = -1;
	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		result = SDL_JoystickInstanceID(m_Devices[InputDeviceIndex].joystick);
	}
	return result;
}

FString DeviceSDL::getDeviceGUIDtoString(int32 InputDeviceIndex)
{
	char buffer[32];
	int8 sizeBuffer = sizeof(buffer);
	FString result = "";

	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(InputDeviceIndex);
		SDL_JoystickGetGUIDString(guid, buffer, sizeBuffer);
		result = ANSI_TO_TCHAR(buffer);
	}
	return result;
}

FGuid DeviceSDL::getDeviceGUIDtoGUID(int32 InputDeviceIndex)
{
	FGuid result;

	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(InputDeviceIndex);
		memcpy(&result, &guid, sizeof(FGuid));
	}
	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfAxes(int32 InputDeviceIndex)
{
	int32 result = 0;

	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumAxes(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfButtons(int32 InputDeviceIndex)
{
	int32 result = 0;

	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumButtons(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfHats(int32 InputDeviceIndex)
{
	int32 result = 0;
	
	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
		if (m_Devices[InputDeviceIndex].isJoystick) {
			result = SDL_JoystickNumHats(m_Devices[InputDeviceIndex].joystick);
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

int32 DeviceSDL::getNumberOfBalls(int32 InputDeviceIndex)
{
	int32 result = 0;

	if ((InputDeviceIndex >= 0) || (InputDeviceIndex < m_NumberOfDevices)) {
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

bool DeviceSDL::getDeviceState(FJoystickState &InputData, FJoystickInfo &JoystickInfo, int32 InputDeviceIndex)
{
	bool result = false;

	if ((InputDeviceIndex < 0) || (InputDeviceIndex >= m_NumberOfDevices)) {
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
		InputData.ButtonsArray.Empty(0);
		for (int64 iButton = 0; iButton < InputData.NumberOfButtons; iButton++) {
			InputData.ButtonsArray.Add(0);
			Uint8 buttonState = SDL_JoystickGetButton(device.joystick, iButton);
			InputData.ButtonsArray[iButton] = buttonState;
		}

		// HATS
		for (int iHat = 0; iHat < InputData.NumberOfHats; iHat++) {
			InputData.HatsArray.Add(0);
			Uint8 hatState = SDL_JoystickGetHat(device.joystick, iHat);
			InputData.HatsArray[iHat] = SDL_hatValToDirection(hatState);
			InputData.POV[iHat] = SDL_hatValToDirection(hatState);
		}

		//BALLS
		InputData.BallsArray.Empty(0);
		for (int64 iBall = 0; iBall < InputData.NumberOfBalls; ++iBall) {
			InputData.BallsArray.Add(0);
			int dx = 0.0f;
			int dy = 0.0f;
			Uint8 ballState = SDL_JoystickGetBall(device.joystick, iBall, &dx, &dy);
			InputData.BallsArray[iBall] = ballState;
		}
	}
	if (device.isGameController) {
		SDL_GameControllerUpdate();
	}
	
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() %s"), device.strName));

	InputData.Player = device.deviceNumber;

	// this is manipulate while init (remove spaces)
	InputData.DeviceName = JoystickInfo.DeviceName;

	return result;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool DeviceSDL::getDeviceSDL(int32 iDevice, sDeviceInfoSDL &deviceInfo)
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

void DeviceSDL::update(float DeltaTime)
{
	// This will spam the log heavily, comment it out for real plugins :)
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::update %f"), DeltaTime);
	int deviceIndex = -1;

	if (SDL_PollEvent(&m_Event_SDL)) {
		switch (m_Event_SDL.type) {
		case SDL_JOYDEVICEADDED:
		case SDL_CONTROLLERDEVICEADDED:
			UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEADDED OR SDL_CONTROLLERDEVICEADDED"));
			
			deviceIndex = m_Event_SDL.cdevice.which;
			g_HotPlugDelegate->JoystickPluggedIn(deviceIndex);
			
			break;
		case SDL_JOYDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMOVED:
			UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEREMOVED OR SDL_CONTROLLERDEVICEREMOVED"));
			deviceIndex = m_Event_SDL.cdevice.which;
			g_HotPlugDelegate->JoystickUnplugged(deviceIndex);
			
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

