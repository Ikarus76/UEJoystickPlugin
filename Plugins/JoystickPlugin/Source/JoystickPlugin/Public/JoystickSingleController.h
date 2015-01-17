#pragma once

#include "JoystickDelegate.h"
#include "JoystickSingleController.generated.h"

UCLASS(BlueprintType)
class UJoystickSingleController : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	int32 Player();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	int64 ButtonsPressedLow();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	int64 ButtonsPressedHigh();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	bool ButtonPressed(int32 number);

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FVector Axis();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FVector RAxis();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	TEnumAsByte<JoystickPOVDirection> POV(int32 index);

	//Obtain POV axis
	UFUNCTION(BlueprintCallable, Category = "Joystick Frame")
	FVector2D POVAxis(int32 index);

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FVector2D Slider();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	bool IsConnected();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FGuid ProductId();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FName ProductName();

	UFUNCTION(BlueprintPure, Category = "Joystick Frame")
	FName InstanceName();

	//blueprint Force feedback function here probably passed through plugin to DirectInput force feedback
	//Call on FF devices to emit force feedback, will do nothing if your device doesn't support FF
	//UFUNCTION(BlueprintCallable, Category = "Joystick Frame")
	//void ForceFeedback(float x, float y, float scale = 1.f);

	void Init(int player, const JoystickData &data, const JoystickInfo &info);

private:
	int player;
	JoystickData data;
	JoystickInfo info;
};
