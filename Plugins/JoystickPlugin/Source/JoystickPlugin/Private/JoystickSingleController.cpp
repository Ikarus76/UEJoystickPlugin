#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"
//#include <sixense.h>

UJoystickSingleController::UJoystickSingleController(const FPostConstructInitializeProperties &init) : UObject(init)
{

}

void UJoystickSingleController::setFromJoystickDataUE(joystickControllerDataUE* data)
{
	this->ButtonsPressed = data->buttonsPressed;

	this->XAxis = data->XAxis;
	this->YAxis = data->YAxis;
	this->ZAxis = data->ZAxis;
	
	this->RXAxis = data->RXAxis;
	this->RYAxis = data->RYAxis;
	this->RZAxis = data->RZAxis;

	this->POV1 = data->POV1;
	
	this->Slider1 = data->Slider1;
}

void UJoystickSingleController::Reset()
{
	this->ButtonsPressed = 0;

	this->XAxis = 0;
	this->YAxis = 0;
	this->ZAxis = 0;

	this->RXAxis = 0;
	this->RYAxis = 0;
	this->RZAxis = 0;

	this->POV1 = 0;

	this->Slider1 = 0;
}