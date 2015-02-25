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

	SDL_Init(0);
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init 0"));

	int result = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	if (result == 0)
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem gamecontroller"));
	}

	result = SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	if (result == 0)
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem joystick"));
	}

	result = SDL_InitSubSystem(SDL_INIT_HAPTIC);
	if (result == 0)
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::InitSDL() SDL init subsystem haptic"));
	}

	SDL_JoystickEventState(SDL_ENABLE);
}

FDeviceSDL::~FDeviceSDL()
{
	UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL Closing"));

	for (auto & Device : Devices)
	{
		DoneDevice(Device.Key);
	}
}

FDeviceInfoSDL * FDeviceSDL::GetDevice(FDeviceId DeviceId)
{
	if (Devices.Contains(DeviceId))
	{
		return &Devices[DeviceId];
	}
	return nullptr;
}

bool FDeviceSDL::InitDevice(FDeviceIndex DeviceIndex, FDeviceInfoSDL &OutDeviceInfo)
{
	OutDeviceInfo.DeviceIndex = DeviceIndex;

	OutDeviceInfo.Joystick = SDL_JoystickOpen(DeviceIndex.value);
	if (OutDeviceInfo.Joystick == nullptr)
		return false;
	OutDeviceInfo.InstanceId = FInstanceId(SDL_JoystickInstanceID(OutDeviceInfo.Joystick));

	// DEBUG
	OutDeviceInfo.Name = FString(ANSI_TO_TCHAR(SDL_JoystickName(OutDeviceInfo.Joystick)));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *OutDeviceInfo.Name);
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Axis %i"), SDL_JoystickNumAxes(OutDeviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Balls %i"), SDL_JoystickNumBalls(OutDeviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Buttons %i"), SDL_JoystickNumButtons(OutDeviceInfo.Joystick));
	UE_LOG(JoystickPluginLog, Log, TEXT("--- Number of Hats %i"), SDL_JoystickNumHats(OutDeviceInfo.Joystick));

	OutDeviceInfo.bIsConnected = true;

	if (SDL_IsGameController(DeviceIndex.value)) 
	{
		OutDeviceInfo.GameController = SDL_GameControllerOpen(DeviceIndex.value);

		OutDeviceInfo.Name = FString(ANSI_TO_TCHAR(SDL_GameControllerName(OutDeviceInfo.GameController)));
		UE_LOG(JoystickPluginLog, Log, TEXT("--- %s"), *OutDeviceInfo.Name);
	}
	
	OutDeviceInfo.Haptic = SDL_HapticOpen(DeviceIndex.value);
	if (OutDeviceInfo.Haptic != nullptr) 
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("--- Rumble device detected"));
		
		if (SDL_HapticRumbleInit(OutDeviceInfo.Haptic) != 0) 
		{
			/*UE_LOG(JoystickPluginLog, Log, TEXT("--- testing Rumble device:"));
			if (SDL_HapticRumblePlay(OutDeviceInfo.Haptic, 0.5, 2000) != 0)
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("--- play Rumble ...."));
				SDL_Delay(2000);
			}
			else
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("--- not successful!"));
				SDL_HapticClose(OutDeviceInfo.Haptic);
				OutDeviceInfo.Haptic = nullptr;
			}*/
		}
	}

	for (auto &ExistingDevice : Devices)
	{
		if (!ExistingDevice.Value.bIsConnected && ExistingDevice.Value.Name == OutDeviceInfo.Name)
		{
			OutDeviceInfo.DeviceId = ExistingDevice.Key;
			Devices[OutDeviceInfo.DeviceId] = OutDeviceInfo;
			DeviceMapping.Add(OutDeviceInfo.InstanceId, OutDeviceInfo.DeviceId);
			return true;
		}
	}

	OutDeviceInfo.DeviceId = FDeviceId(Devices.Num());
	Devices.Add(OutDeviceInfo.DeviceId, OutDeviceInfo);
	DeviceMapping.Add(OutDeviceInfo.InstanceId, OutDeviceInfo.DeviceId);

	return true;
}

void FDeviceSDL::DoneDevice(FDeviceId DeviceId)
{
	FDeviceInfoSDL &DeviceInfo = Devices[DeviceId];

	if (DeviceInfo.Haptic != nullptr)
	{
		SDL_HapticClose(DeviceInfo.Haptic);
		DeviceInfo.Haptic = nullptr;
	}

	if (DeviceInfo.Joystick != nullptr)
	{
		SDL_JoystickClose(DeviceInfo.Joystick);
		DeviceInfo.Joystick = nullptr;
		DeviceInfo.bIsConnected = false;
	}

	if (DeviceInfo.GameController != nullptr)
	{
		SDL_GameControllerClose(DeviceInfo.GameController);
		DeviceInfo.GameController = nullptr;
		DeviceInfo.bIsConnected = false;
	}
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

EJoystickPOVDirection SDL_hatValToDirection(int8 Value)
{
	switch (Value)
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
	if (!Devices.Contains(DeviceId))
	{
		UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() ERROR index out of range"));
		return false;
	}

	FDeviceInfoSDL Device = Devices[DeviceId];

	if (Device.Joystick)
	{
		SDL_JoystickUpdate();

		OutInputData.NumberOfBalls = SDL_JoystickNumBalls(Device.Joystick);
		OutInputData.NumberOfHats = SDL_JoystickNumHats(Device.Joystick);
		OutInputData.NumberOfButtons = SDL_JoystickNumButtons(Device.Joystick);
		OutInputData.NumberOfAxis = SDL_JoystickNumAxes(Device.Joystick);

		// clear TArray add zero change value
		// AXES
		OutInputData.AxisArray.Empty(0);
		float scaledValue = 0;
		for (int iAxes = 0; iAxes < OutInputData.NumberOfAxis; iAxes++)
		{
			OutInputData.AxisArray.Add(0);
			int rawValue = SDL_JoystickGetAxis(Device.Joystick, iAxes);
			if (rawValue < 0)
			{
				scaledValue = rawValue / 32768.0;
			}
			else 
			{
				scaledValue = rawValue / 32767.0;
			}
			OutInputData.AxisArray[iAxes] = scaledValue;
		}

		// BUTTONS
		OutInputData.ButtonsArray.Empty(OutInputData.NumberOfButtons);
		for (int iButton = 0; iButton < OutInputData.NumberOfButtons; iButton++)
		{
			OutInputData.ButtonsArray.Add(SDL_JoystickGetButton(Device.Joystick, iButton));
		}

		// HATS
		OutInputData.HatsArray.Empty(OutInputData.NumberOfHats);
		OutInputData.POV.Empty(OutInputData.NumberOfHats);
		for (int iHat = 0; iHat < OutInputData.NumberOfHats; iHat++)
		{
			Uint8 hatState = SDL_JoystickGetHat(Device.Joystick, iHat);
			OutInputData.HatsArray.Add(SDL_hatValToDirection(hatState));
			OutInputData.POV.Add(SDL_hatValToDirection(hatState));
		}
	}
	if (Device.GameController)
	{
		SDL_GameControllerUpdate();
	}
	
	//UE_LOG(JoystickPluginLog, Log, TEXT("DeviceSDL::getDeviceState() %s"), device.Name));

	// this is manipulated during init (remove spaces)
	OutInputData.DeviceName = JoystickInfo.DeviceName;

	return true;
}

void FDeviceSDL::Update()
{
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
			case SDL_JOYDEVICEADDED:
			{
				FDeviceIndex deviceIndex = FDeviceIndex(Event.cdevice.which);
				EventInterface->JoystickPluggedIn(deviceIndex);
				break;
			}
			case SDL_JOYDEVICEREMOVED:
			{
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

