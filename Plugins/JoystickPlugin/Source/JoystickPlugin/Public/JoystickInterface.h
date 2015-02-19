#pragma once

#include "JoystickInterface.generated.h"

UENUM(BlueprintType)
enum EInputType
{
	INPUTTYPE_UNKNOWN,
	INPUTTYPE_JOYSTICK,
	INPUTTYPE_GAMECONTROLLER,	
};


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

FVector2D POVAxis(JoystickPOVDirection povValue);

USTRUCT(BlueprintType)
struct FJoystickState
{
	GENERATED_USTRUCT_BODY()

	explicit FJoystickState(int deviceId = -1)
	: DeviceId(static_cast<int>(deviceId))
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	FString DeviceName;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 DeviceId;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 NumberOfHats;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	TArray<int32> HatsArray;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 NumberOfBalls;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 NumberOfAxis;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	TArray<float> AxisArray;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 NumberOfButtons;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	TArray<int32> ButtonsArray;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	TArray<TEnumAsByte<JoystickPOVDirection>> POV;
/*
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
		FVector2D Slider = FVector2D(0, 0);
*/

};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FJoystickInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	int32 Player = -1;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	int32 DeviceId = -1;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	bool IsGameController = false;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	bool IsRumbleDevice = false;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FGuid InstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FGuid ProductId;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FName ProductName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FName InstanceName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FString DeviceName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	bool Connected = false;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	TArray<TEnumAsByte<EInputType>> InputType;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

UINTERFACE(MinimalAPI)
class UJoystickInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class IJoystickInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	//Define blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonPressed(int32 ButtonNr, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonReleased(int32 ButtonNr, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickAxisArrayChanged(int32 index, float value, float valuePrev, FJoystickState state, FJoystickState prev);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonsArrayChanged(int32 index, bool value, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickHatsArrayChanged(int32 index, float value, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickBallsArrayChanged(int32 index, int32 dx, int32 dy, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickPluggedIn(int32 iDevice);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickUnplugged(int32 iDevice);

	virtual FString ToString();
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
