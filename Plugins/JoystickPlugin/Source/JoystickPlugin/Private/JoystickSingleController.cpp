#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"

#define HALF_RANGE 32768
#define JOYSTICK_SCALING_FACTOR 65536
#define INVERSE_SCALING_FACTOR 0.00001525878	//this is 1/65536

UJoystickSingleController::UJoystickSingleController(const FPostConstructInitializeProperties &init) : UObject(init)
{

}

FVector normalizeAxis(FVector in)
{
	return ((in - HALF_RANGE)) / HALF_RANGE;
}

FVector2D normalizeSlider(FVector2D value)
{
	return (value - HALF_RANGE) / HALF_RANGE;
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
		UE_LOG(LogTemp, Warning, TEXT("Warning, POV unhandled case. %d"), (int32)value);
		return DIRECTION_NONE;
	}
}


void UJoystickSingleController::setFromJoystickDataUE(joystickControllerDataUE* data)
{
	this->ButtonsPressedLow = data->buttonsPressedL;
	this->ButtonsPressedHigh = data->buttonsPressedH;

	this->Axis = normalizeAxis(data->Axis);
	this->RAxis = normalizeAxis(data->RAxis); 
	
	//this->POV1 = data->POV1;

	this->POV0 = povValToDirection(data->POV.X);
	this->POV1 = povValToDirection(data->POV.Y);
	this->POV2 = povValToDirection(data->POV.Z);

	this->Slider = normalizeSlider(data->Slider);

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