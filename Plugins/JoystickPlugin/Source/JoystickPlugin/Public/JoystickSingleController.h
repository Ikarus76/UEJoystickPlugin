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
	int64 ButtonsPressedLow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	int64 ButtonsPressedHigh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	FVector Axis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	FVector RAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	TEnumAsByte<JoystickPOVDirection> POV0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	TEnumAsByte<JoystickPOVDirection> POV1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	TEnumAsByte<JoystickPOVDirection> POV2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	FVector2D Slider;

	//blueprint Force feedback function here probably passed through plugin to DirectInput force feedback

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Joystick Frame")
	bool IsValid;

	//Obtain POV axis
	UFUNCTION(BlueprintCallable, Category = "Joystick Frame")
	FVector2D POVAxis(POVIndex Index);

	//Call on FF devices to emit force feedback, will do nothing if your device doesn't support FF
	//UFUNCTION(BlueprintCallable, Category = "Joystick Frame")
	//void ForceFeedback(float x, float y, float scale = 1.f);

	void Reset();
	void setFromJoystickDataUE(joystickControllerDataUE* data);
};