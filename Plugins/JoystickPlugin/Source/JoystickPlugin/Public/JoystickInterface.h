#pragma once

#include "JoystickInterface.generated.h"

UENUM(BlueprintType)
enum JoystickPOVDirection
{
	DIRECTION_NONE,
	DIRECTION_UP,
	DIRECTION_UP_RIGHT,
	DIRECTION_RIGHT,
	DIRECTION_DOWN_RIGHT,
	DIRECTION_DOWN,
	DIRECTION_DOWN_LEFT,
	DIRECTION_LEFT,
	DIRECTION_UP_LEFT,
};

UENUM(BlueprintType)
enum POVIndex
{
	POV_1,
	POV_2,
	POV_3
};

UINTERFACE(MinimalAPI)
class UJoystickInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IJoystickInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	//Define blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonPressed(int32 ButtonNr, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonReleased(int32 ButtonNr, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickAxisChanged(FVector AxisValue, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickRAxisChanged(FVector AxisValue, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickPOVChanged(JoystickPOVDirection POVDirection, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickSliderChanged(FVector2D SliderValue, class UJoystickSingleController* controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickPluggedIn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickUnplugged();

	virtual FString ToString();
};