#include "JoystickPluginPrivatePCH.h"
#include "JoystickBlueprintDelegate.h"

void JoystickBlueprintDelegate::JoystickButtonPressed(int32 buttonNr)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonPressed(_interfaceDelegate, buttonNr);
}

void JoystickBlueprintDelegate::JoystickButtonReleased(int32 buttonNr)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickButtonReleased(_interfaceDelegate, buttonNr);
}

void JoystickBlueprintDelegate::XAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickXAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::YAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickYAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::ZAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickZAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::RXAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickRXAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::RYAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickRYAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::RZAxisChanged(int32 AxisValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickRZAxisChanged(_interfaceDelegate, AxisValue);
}

void JoystickBlueprintDelegate::POV1Changed(int32 POVValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickPOV1Changed(_interfaceDelegate, POVValue);
}

void JoystickBlueprintDelegate::Slider1Changed(int32 SliderValue)
{
	if (implementsInterface())
		IJoystickInterface::Execute_JoystickSlider1Changed(_interfaceDelegate, SliderValue);
}



//Blueprint exposing the HydraDelegate Methods
//Override Callable Functions - Required to forward their implementations in order to compile, cannot skip implementation as we can for events.
bool JoystickBlueprintDelegate::JoystickIsAvailable()
{
	return JoystickDelegate::JoystickIsAvailable();
}

UJoystickSingleController* JoystickBlueprintDelegate::JoystickGetLatestFrame()
{
	return _latestFrame;
}

bool JoystickBlueprintDelegate::implementsInterface()
{
	return (_interfaceDelegate != NULL && _interfaceDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()));
}

//Setting
void JoystickBlueprintDelegate::SetInterfaceDelegate(UObject* newDelegate)
{
	UE_LOG(LogClass, Log, TEXT("InterfaceDelegate passed: %s"), *newDelegate->GetName());

	//Use this format to support both blueprint and C++ form
	if (newDelegate->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
	{
		_interfaceDelegate = newDelegate;
	}
	else
	{
		//Try casting as self
		if (ValidSelfPointer->GetClass()->ImplementsInterface(UJoystickInterface::StaticClass()))
		{
			_interfaceDelegate = (UObject*)this;
		}
		else
		{
			//If you're crashing its probably because of this setting causing an assert failure
			_interfaceDelegate = NULL;
			UE_LOG(LogClass, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface delegate is NULL, did your class implement JoystickInterface? Events are disabled."));
		}

		//Either way post a warning, this will be a common error
		UE_LOG(LogClass, Log, TEXT("JoystickBlueprintDelegate Warning: JoystickInterface Delegate is NOT set, did your class implement JoystickInterface? Events are disabled."));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("JoystickBlueprintDelegate Warning: Delegate is NOT set, did your class implement JoystickInterface? Events are disabled."));
	}
}

//Startup
void JoystickBlueprintDelegate::JoystickStartup()
{
	JoystickDelegate::JoystickStartup();

	UObject* validUObject = NULL;
	validUObject = Cast<UObject>(ValidSelfPointer);

	//Setup our Controller BP
	UJoystickSingleController* controller;
	if (validUObject)
		controller = NewObject<UJoystickSingleController>(validUObject, UJoystickSingleController::StaticClass());
	else
		controller = NewObject<UJoystickSingleController>(UJoystickSingleController::StaticClass());	//no ownership
	controller->Reset();
	_latestFrame = controller;


	//Set self as interface delegate by default
	if (!_interfaceDelegate && validUObject)
		SetInterfaceDelegate(validUObject);
}


void JoystickBlueprintDelegate::JoystickTick(float DeltaTime)
{
	JoystickDelegate::JoystickTick(DeltaTime);

	//Sync 
	UJoystickSingleController* controller = _latestFrame;

	int32 prevButtons = controller->ButtonsPressed;
	int32 prevX = controller->XAxis;
	int32 prevY = controller->YAxis;
	int32 prevZ = controller->ZAxis;
	int32 prevRX = controller->RXAxis;
	int32 prevRY = controller->RYAxis;
	int32 prevRZ = controller->RZAxis;
	int32 prevP1 = controller->POV1;
	int32 prevS1 = controller->Slider1;

	controller->setFromJoystickDataUE(JoystickDelegate::JoystickGetLatestData());
	
	// check buttons
	int32 bitVal = 0;
	for (int32 i = 0; i < 128; i++)
	{
		if (i == 0) bitVal = 1;
		else bitVal = pow((double)2, i);

		if (false == ((prevButtons & bitVal) == (controller->ButtonsPressed & bitVal)))
		{
			if (controller->ButtonsPressed & bitVal)
				JoystickButtonPressed(i + 1);
			else
				JoystickButtonReleased(i + 1);
		}
	}

	// check axis
	if (prevX != controller->XAxis) XAxisChanged(controller->XAxis);
	if (prevY != controller->YAxis) YAxisChanged(controller->YAxis);
	if (prevZ != controller->ZAxis) ZAxisChanged(controller->ZAxis);

	if (prevRX != controller->RXAxis) RXAxisChanged(controller->RXAxis);
	if (prevRY != controller->RYAxis) RYAxisChanged(controller->RYAxis);
	if (prevRZ != controller->RZAxis) RZAxisChanged(controller->RZAxis);

	if (prevP1 != controller->POV1) POV1Changed(controller->POV1);

	if (prevS1 != controller->Slider1) Slider1Changed(controller->Slider1);

	_latestFrame = controller;
}