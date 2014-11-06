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
float normalizeValue(float value)
{
	return (value - HALF_RANGE) / HALF_RANGE;
}

void UJoystickSingleController::setFromJoystickDataUE(joystickControllerDataUE* data)
{
	this->ButtonsPressed = data->buttonsPressed;

	this->Axis = normalizeAxis(data->Axis);
	this->RAxis = normalizeAxis(data->RAxis); 
	
	//this->POV1 = data->POV1;

	switch (data->POV1){
	case -1:
		this->POV = DIRECTION_NONE;
		break;
	case 0:
		this->POV = DIRECTION_UP;
		break;
	case 4500:
		this->POV = DIRECTION_UP_RIGHT;
		break;
	case 9000:
		this->POV = DIRECTION_RIGHT;
		break;
	case 13500:
		this->POV = DIRECTION_DOWN_RIGHT;
		break;
	case 18000:
		this->POV = DIRECTION_DOWN;
		break;
	case 22500:
		this->POV = DIRECTION_DOWN_LEFT;
		break;
	case 27000:
		this->POV = DIRECTION_LEFT;
		break;
	case 31500:
		this->POV = DIRECTION_UP_LEFT;
		break;
	default:
		this->POV = DIRECTION_NONE;
		UE_LOG(LogTemp, Warning, TEXT("Warning, POV unhandled case. %d"), data->POV1);
		break;
	}

	this->Slider = normalizeValue(data->Slider1);
	this->IsValid = true;
}

FVector2D UJoystickSingleController::POVAxis()
{
	switch (this->POV){
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
	this->ButtonsPressed = 0;
	this->Axis = FVector(0, 0, 0);
	this->RAxis = FVector(0, 0, 0);
	this->POV = DIRECTION_NONE;
	this->Slider = 0;
	this->IsValid = false;
}