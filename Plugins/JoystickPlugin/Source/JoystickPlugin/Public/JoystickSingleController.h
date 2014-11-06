#pragma once

#include "JoystickDelegate.h"
#include "JoystickSingleController.generated.h"

struct joystickControllerDataUE;

UCLASS(BlueprintType)
class UJoystickSingleController : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int32 ButtonsPressed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	FVector Axis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	FVector RAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	TEnumAsByte<JoystickPOVDirection> POV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	float Slider;

	//blueprint Force feedback function here probably passed through plugin to DirectInput force feedback

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	bool IsValid;

	//Obtain POV axis
	UFUNCTION(BlueprintCallable, Category = "Joystick Frame")
	FVector2D POVAxis();

	void Reset();
	void setFromJoystickDataUE(joystickControllerDataUE* data);
};