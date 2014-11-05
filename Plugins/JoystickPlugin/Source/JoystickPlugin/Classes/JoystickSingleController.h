#pragma once

#include "JoystickDelegate.h"
#include "JoystickSingleController.generated.h"

UCLASS(BlueprintType)
class UJoystickSingleController : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 ButtonsPressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 YAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 ZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 RXAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 RYAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 RZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 POV1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 Slider1;

	void Reset();

	void setFromJoystickDataUE(joystickControllerDataUE* data);
};