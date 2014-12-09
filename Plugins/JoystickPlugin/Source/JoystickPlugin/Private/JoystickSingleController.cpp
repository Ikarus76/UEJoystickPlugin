#include "JoystickPluginPrivatePCH.h"
#include "IJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"

UJoystickSingleController::UJoystickSingleController(const FObjectInitializer &init) : UObject(init)
{
	Reset();
}

JoystickPOVDirection povValToDirection(float value)
{
	switch ((int32)value){
	case -1:	return DIRECTION_NONE;
	case 0:		return DIRECTION_UP;
	case 4500:	return DIRECTION_UP_RIGHT;
	case 9000:	return DIRECTION_RIGHT;
	case 13500:	return DIRECTION_DOWN_RIGHT;
	case 18000:	return DIRECTION_DOWN;
	case 22500:	return DIRECTION_DOWN_LEFT;
	case 27000:	return DIRECTION_LEFT;
	case 31500:	return DIRECTION_UP_LEFT;
	default:
		//UE_LOG(LogTemp, Warning, TEXT("Warning, POV unhandled case. %d"), (int32)value);
		return DIRECTION_NONE;
	}
}


void UJoystickSingleController::setFromJoystickDataUE(joystickControllerDataUE* data)
{
	this->ButtonsPressedLow = data->buttonsPressedL;
	this->ButtonsPressedHigh = data->buttonsPressedH;

	this->Axis = JoystickUtilityNormalizeAxis(data->Axis);
	this->RAxis = JoystickUtilityNormalizeAxis(data->RAxis);
	
	//this->POV1 = data->POV1;

	this->POV0 = povValToDirection(data->POV.X);
	this->POV1 = povValToDirection(data->POV.Y);
	this->POV2 = povValToDirection(data->POV.Z);

	this->Slider = JoystickUtilityNormalizeSlider(data->Slider);

	this->IsValid = true;
}

FVector2D UJoystickSingleController::POVAxis(POVIndex Index)
{
	TEnumAsByte<JoystickPOVDirection> povValue = DIRECTION_NONE;

	switch (Index)
	{
	case POV_1:
		povValue = POV0;
		break;
	case POV_2:
		povValue = POV1;
		break;
	case POV_3:
		povValue = POV2;
		break;
	default:
		break;
	}

	switch (povValue){
	case DIRECTION_NONE:
		return FVector2D(0, 0);
		break;
	case DIRECTION_UP:
		return FVector2D(0, 1);
		break;
	case DIRECTION_UP_RIGHT:
		return FVector2D(1, 1);
		break;
	case DIRECTION_RIGHT:
		return FVector2D(1, 0);
		break;
	case DIRECTION_DOWN_RIGHT:
		return FVector2D(1, -1);
		break;
	case DIRECTION_DOWN:
		return FVector2D(0, -1);
		break;
	case DIRECTION_DOWN_LEFT:
		return FVector2D(-1, -1);
		break;
	case DIRECTION_LEFT:
		return FVector2D(-1, 0);
		break;
	case DIRECTION_UP_LEFT:
		return FVector2D(-1, 1);
		break;
	default:
		return FVector2D(0, 0);
		break;
	}
}

/*void UJoystickSingleController::ForceFeedback(float x, float y, float scale)
{
	//Get the plugin singleton and forward the force feedback
	if (IJoystickPlugin::IsAvailable())
	{
		UE_LOG(LogTemp, Warning, TEXT("Denormalized values: %d, %d"), JoystickUtilityDeNormalizeForceValue(x), JoystickUtilityDeNormalizeForceValue(y));
		IJoystickPlugin::Get().ForceFeedbackXY(JoystickUtilityDeNormalizeForceValue(x), JoystickUtilityDeNormalizeForceValue(y), scale);
	}
}*/

void UJoystickSingleController::Reset()
{
	this->ButtonsPressedLow = 0;
	this->ButtonsPressedHigh = 0;
	this->Axis = FVector(0, 0, 0);
	this->RAxis = FVector(0, 0, 0);
	this->POV0 = DIRECTION_NONE;
	this->POV1 = DIRECTION_NONE;
	this->POV2 = DIRECTION_NONE;
	this->Slider = FVector2D::ZeroVector;
	this->IsValid = false;
}