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

FVector2D POVAxis(JoystickPOVDirection povValue);

USTRUCT(BlueprintType)
struct FJoystickState
{
	GENERATED_USTRUCT_BODY()

	explicit FJoystickState(int32 player = -1)
	: Player(player)
	{
		for (int i = 0; i < 4; i++)
		{
			POV.Add(DIRECTION_NONE);
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	int32 Player;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	uint64 buttonsPressedL = 0;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	uint64 buttonsPressedH = 0;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	FVector Axis = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	FVector RAxis = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	TArray<TEnumAsByte<JoystickPOVDirection>> POV;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickState)
	FVector2D Slider = FVector2D(0, 0);
};

USTRUCT(BlueprintType)
struct FJoystickInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	int32 Player = -1;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FGuid InstanceId;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FGuid ProductId;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FName ProductName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	FName InstanceName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = JoystickInfo)
	bool Connected = false;
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
	void JoystickButtonPressed(int32 ButtonNr, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickButtonReleased(int32 ButtonNr, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickAxisChanged(FVector AxisValue, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickRAxisChanged(FVector AxisValue, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickPOVChanged(JoystickPOVDirection POVDirection, int32 index, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickSliderChanged(FVector2D SliderValue, FJoystickState state);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickPluggedIn(FJoystickInfo joystick);

	UFUNCTION(BlueprintImplementableEvent, Category = "Joystick Interface Events")
	void JoystickUnplugged(FJoystickInfo joystick);

	virtual FString ToString();
};
