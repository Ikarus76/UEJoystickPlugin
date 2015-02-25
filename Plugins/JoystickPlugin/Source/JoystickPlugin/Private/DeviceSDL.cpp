/*
*
* Copyright (C) <2014> samiljan <Sam Persson>, tsky <thomas.kollakowksy@w-hs.de>
* All rights reserved.
*
* This software may be modified and distributed under the terms
* of the BSD license.  See the LICENSE file for details.
*/

#include "JoystickPluginPrivatePCH.h"
#include "DeviceSDL.h"
#include "JoystickDevice.h"

DEFINE_LOG_CATEGORY(JoystickPluginLog);

FDeviceSDL::FDeviceSDL(IJoystickEventInterface * eventInterface) :
	EventInterface(eventInterface)
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL Starting"));

	InitSDL();
}

FDeviceSDL::~FDeviceSDL()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL Closing"));
	DoneSDL();
}


void FDeviceSDL::InitSDL()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL()"));

	int result;

	SDL_Init(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init 0"));

	result = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem gamecontroller"));
	}

	result = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem joystick"));
	}

	result = SDL_InitSubSystem(SDL_INIT_HAPTIC);
	if (result == 0) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem haptic"));
	}

	SDL_JoystickEventState(SDL_ENABLE);
}

void FDeviceSDL::DoneSDL()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::DoneSDL()"));

	for (auto & Device : Devices) {
		DoneDevice(Device.Key);
	}
}

void FDeviceSDL::ResetDevice(FDeviceId DeviceId)
{	
	if (Devices.Contains(DeviceId)) {
		Devices[DeviceId].DeviceId = DeviceId;
		Devices[DeviceId].GameController = nullptr;
		Devices[DeviceId].Haptic = nullptr;
		Devices[DeviceId].bHasHaptic = false;
		Devices[DeviceId].bIsConnected = false;
		Devices[DeviceId].bIsGameController = false;
		Devices[DeviceId].bIsJoystick = false;
		Devices[DeviceId].Joystick = nullptr;
		Devices[DeviceId].Name = "unknown";
	}	
}

FDeviceInfoSDL * FDeviceSDL::GetDevice(FDeviceId iDevice)
{
	if (Devices.Contains(iDevice))
	{
		return &Devices[iDevice];
	}
	return nullptr;
}

bool FDeviceSDL::InitDevice(FDeviceIndex iDevice, FDeviceInfoSDL &deviceInfo)
{
	deviceInfo.DeviceIndex = iDevice;

	deviceInfo.Joystick = SDL_JoystickOpen(iDevice.value);
	if (deviceInfo.Joystick == nullptr)
		return false;
	deviceInfo.InstanceId = FInstanceId(SDL_JoystickInstanceID(deviceInfo.Joystick));

	// DEBUG
	deviceInfo.Name = FString(ANSI_TO_TCHAR(SDL_JoystickName(deviceInfo.Joystick)));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *deviceInfo.Name);
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Axis %i"), SDL_JoystickNumAxes(deviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Balls %i"), SDL_JoystickNumBalls(deviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Buttons %i"), SDL_JoystickNumButtons(deviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Hats %i"), SDL_JoystickNumHats(deviceInfo.Joystick));

	deviceInfo.bIsGameController = false;
	deviceInfo.bIsJoystick = true;
	deviceInfo.bIsConnected = true;

	if (SDL_IsGameController(iDevice.value)) {
		deviceInfo.GameController = SDL_GameControllerOpen(iDevice.value);

		deviceInfo.Name = FString(ANSI_TO_TCHAR(SDL_GameControllerName(deviceInfo.GameController)));
		UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *deviceInfo.Name);
		
		deviceInfo.bIsGameController = true;
	}
	
	deviceInfo.Haptic = SDL_HapticOpen(iDevice.value);
	if (deviceInfo.Haptic != nullptr) {
		UE_LOG(JoystickPluginLog, Log, TEXT("--- Rumble device detected"));
		
		if (SDL_HapticRumbleInit(deviceInfo.Haptic) != 0) {
			UE_LOG(JoystickPluginLog, Log, TEXT("--- testing Rumble device:"));
			if (SDL_HapticRumblePlay(deviceInfo.Haptic, 0.5, 2000) != 0) {
				UE_LOG(JoystickPluginLog, Log, TEXT("--- play Rumble ...."));
				SDL_Delay(2000);

				deviceInfo.bHasHaptic = true;
			} else {
				UE_LOG(JoystickPluginLog, Log, TEXT("--- not successful!"));
				deviceInfo.bHasHaptic = false;
			}
		}
	}

	for (auto &existingDevice : Devices)
	{
		if (!existingDevice.Value.bIsConnected && existingDevice.Value.Name == deviceInfo.Name)
		{
			deviceInfo.DeviceId = existingDevice.Key;
			Devices[deviceInfo.DeviceId] = deviceInfo;
			DeviceMapping.Add(deviceInfo.InstanceId, deviceInfo.DeviceId);
			return true;
		}
	}

	deviceInfo.DeviceId = FDeviceId(Devices.Num());
	Devices.Add(deviceInfo.DeviceId, deviceInfo);
	DeviceMapping.Add(deviceInfo.InstanceId, deviceInfo.DeviceId);

	return true;
}

bool FDeviceSDL::DoneDevice(FDeviceId deviceId)
{
	bool result = false;

	FDeviceInfoSDL &deviceInfo = Devices[deviceId];

	if (deviceInfo.Haptic != nullptr) {
		SDL_HapticClose(deviceInfo.Haptic);
		deviceInfo.Haptic = nullptr;
		deviceInfo.bHasHaptic = false;
	}

	if (deviceInfo.Joystick != nullptr) {
		SDL_JoystickClose(deviceInfo.Joystick);
		deviceInfo.Joystick = nullptr;
		deviceInfo.bIsJoystick = false;
		deviceInfo.bIsConnected = false;
		result = true;
	}

	if (deviceInfo.GameController != nullptr) {
		SDL_GameControllerClose(deviceInfo.GameController);
		deviceInfo.GameController = nullptr;
		deviceInfo.bIsGameController = false;
		deviceInfo.bIsConnected = false;
		result = true;
	}

	//resetDevice(deviceInfo.deviceId);

	return result;
}

FString FDeviceSDL::DeviceGUIDtoString(FDeviceIndex DeviceIndex)
{
	char buffer[32];
	int8 sizeBuffer = sizeof(buffer);

	SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(DeviceIndex.value);
	SDL_JoystickGetGUIDString(guid, buffer, sizeBuffer);
	return ANSI_TO_TCHAR(buffer);
}

FGuid FDeviceSDL::DeviceGUIDtoGUID(FDeviceIndex DeviceIndex)
{
	FGuid result;
	SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(DeviceIndex.value);
	memcpy(&result, &guid, sizeof(FGuid));
	return result;
}

EJoystickPOVDirection SDL_hatValToDirection(int8 value)
{
	switch (value)
	{
	case SDL_HAT_CENTERED:  return EJoystickPOVDirection::DIRECTION_NONE;
	case SDL_HAT_UP:        return EJoystickPOVDirection::DIRECTION_UP;
	case SDL_HAT_RIGHTUP:   return EJoystickPOVDirection::DIRECTION_UP_RIGHT;
	case SDL_HAT_RIGHT:	    return EJoystickPOVDirection::DIRECTION_RIGHT;
	case SDL_HAT_RIGHTDOWN: return EJoystickPOVDirection::DIRECTION_DOWN_RIGHT;
	case SDL_HAT_DOWN:	    return EJoystickPOVDirection::DIRECTION_DOWN;
	case SDL_HAT_LEFTDOWN:  return EJoystickPOVDirection::DIRECTION_DOWN_LEFT;
	case SDL_HAT_LEFT:	    return EJoystickPOVDirection::DIRECTION_LEFT;
	case SDL_HAT_LEFTUP:    return EJoystickPOVDirection::DIRECTION_UP_LEFT;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Warning, POV unhandled case. %d"), (int32)value);
		return EJoystickPOVDirection::DIRECTION_NONE;
	}
}

bool FDeviceSDL::GetDeviceState(FJoystickState &OutInputData, const FJoystickInfo &JoystickInfo, FDeviceId DeviceId)
{
	bool result = false;

	if (!Devices.Contains(DeviceId)) {
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() ERROR index out of range"));
		return result;
	}
	
	result = true;

	FDeviceInfoSDL device = Devices[DeviceId];

	if (device.bIsJoystick) {
		SDL_JoystickUpdate();

		OutInputData.NumberOfBalls = SDL_JoystickNumBalls(device.Joystick);
		OutInputData.NumberOfHats = SDL_JoystickNumHats(device.Joystick);
		OutInputData.NumberOfButtons = SDL_JoystickNumButtons(device.Joystick);
		OutInputData.NumberOfAxis = SDL_JoystickNumAxes(device.Joystick);

		// clear TArray add zero change value
		// AXES
		OutInputData.AxisArray.Empty(0);
		float scaledValue = 0;
		for (int iAxes = 0; iAxes < OutInputData.NumberOfAxis; iAxes++) {
			OutInputData.AxisArray.Add(0);
			int rawValue = SDL_JoystickGetAxis(device.Joystick, iAxes);
			if (rawValue < 0) {
				scaledValue = rawValue / 32768.0;
			}
			else {
				scaledValue = rawValue / 32767.0;
			}
			OutInputData.AxisArray[iAxes] = scaledValue;
		}

		// BUTTONS
		OutInputData.ButtonsArray.Empty(OutInputData.NumberOfButtons);
		for (int iButton = 0; iButton < OutInputData.NumberOfButtons; iButton++) {
			OutInputData.ButtonsArray.Add(SDL_JoystickGetButton(device.Joystick, iButton));
		}

		// HATS
		OutInputData.HatsArray.Empty(OutInputData.NumberOfHats);
		OutInputData.POV.Empty(OutInputData.NumberOfHats);
		for (int iHat = 0; iHat < OutInputData.NumberOfHats; iHat++) {
			Uint8 hatState = SDL_JoystickGetHat(device.Joystick, iHat);
			OutInputData.HatsArray.Add(SDL_hatValToDirection(hatState));
			OutInputData.POV.Add(SDL_hatValToDirection(hatState));
		}
	}
	if (device.bIsGameController) {
		SDL_GameControllerUpdate();
	}
	
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() %s"), device.Name));

	// this is manipulate while init (remove spaces)
	OutInputData.DeviceName = JoystickInfo.DeviceName;

	return result;
}

void FDeviceSDL::update()
{
	// This will spam the log heavily, comment it out for real plugins :)
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::update %f"), deltaTime);

	while (SDL_PollEvent(&Event)) {
		switch (Event.type) {
			case SDL_JOYDEVICEADDED:
			{
				//UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEADDED OR SDL_CONTROLLERDEVICEADDED"));
				FDeviceIndex deviceIndex = FDeviceIndex(Event.cdevice.which);
				EventInterface->JoystickPluggedIn(deviceIndex);
				break;
			}
			case SDL_JOYDEVICEREMOVED:
			{
				//UE_LOG(JoystickPluginLog, Log, TEXT("SDL_JOYDEVICEREMOVED OR SDL_CONTROLLERDEVICEREMOVED"));
				FInstanceId instanceId = FInstanceId(Event.cdevice.which);
				FDeviceId deviceId = DeviceMapping[instanceId];
				DeviceMapping.Remove(instanceId);
				EventInterface->JoystickUnplugged(deviceId);
				break;
			}
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
			{
				FDeviceId deviceId = DeviceMapping[FInstanceId(Event.jbutton.which)];
				EventInterface->JoystickButton(deviceId, Event.jbutton.button, Event.jbutton.state == SDL_PRESSED);
				break;
			}
			case SDL_JOYAXISMOTION:
			{
				FDeviceId deviceId = DeviceMapping[FInstanceId(Event.jaxis.which)];
				EventInterface->JoystickAxis(deviceId, Event.jaxis.axis, Event.jaxis.value / (Event.jaxis.value < 0 ? 32768.0f : 32767.0f));
				break;
			}
			case SDL_JOYHATMOTION:
			{
				FDeviceId deviceId = DeviceMapping[FInstanceId(Event.jhat.which)];
				EventInterface->JoystickHat(deviceId, Event.jhat.hat, SDL_hatValToDirection(Event.jhat.value));
				break;
			}
			case SDL_JOYBALLMOTION:
			{
				FDeviceId deviceId = DeviceMapping[FInstanceId(Event.jball.which)];
				EventInterface->JoystickBall(deviceId, Event.jball.ball, Event.jball.xrel, Event.jball.yrel);
				break;
			}
		}
	}
}

