#include "JoystickPluginPrivatePCH.h"

#include "IJoystickPlugin.h"

#include "FJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "JoystickSingleController.h"
#include "Slate.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <windows.h>

#include "WinJoystick.h"

IMPLEMENT_MODULE(FJoystickPlugin, JoystickPlugin)

#define LOCTEXT_NAMESPACE "JoystickPlugin"

//Collector class contains all the data captured from .dll and delegate data will point to this structure (allDataUE and historicalDataUE).
class DataCollector
{
public:

	joystickControllerDataUE* currDataUE;
	joystickControllerDataUE* prevDataUE;
	UJoystickSingleController* currentController;
	UJoystickSingleController* prevController;

	DataCollector()
	{
		currDataUE = new joystickControllerDataUE;
		ZeroMemory(currDataUE, sizeof(joystickControllerDataUE));
		prevDataUE = new joystickControllerDataUE;
		ZeroMemory(prevDataUE, sizeof(joystickControllerDataUE));

		currentController = NewObject<UJoystickSingleController>(UJoystickSingleController::StaticClass());
		prevController = NewObject<UJoystickSingleController>(UJoystickSingleController::StaticClass());
	}
	~DataCollector()
	{
		delete currDataUE;
		delete prevDataUE;
		//currentController->ConditionalBeginDestroy();
		//prevController->ConditionalBeginDestroy();
	}

	void UpdateData(joystickControllerDataUE* newData)
	{
		memcpy(prevDataUE, currDataUE, sizeof(joystickControllerDataUE));
		memcpy(currDataUE, newData, sizeof(joystickControllerDataUE));

		currentController->setFromJoystickDataUE(currDataUE);
		prevController->setFromJoystickDataUE(prevDataUE);
	}
};

//Init and Runtime
void FJoystickPlugin::StartupModule()
{
	UE_LOG(LogClass, Log, TEXT("Attempting to startup Joystick Module."));

	//Add the keys either way, these should always be available when plugin starts up
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton1, LOCTEXT("JoystickButton1", "Joystick Button 1"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton2, LOCTEXT("JoystickButton2", "Joystick Button 2"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton3, LOCTEXT("JoystickButton3", "Joystick Button 3"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton4, LOCTEXT("JoystickButton4", "Joystick Button 4"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton5, LOCTEXT("JoystickButton5", "Joystick Button 5"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton6, LOCTEXT("JoystickButton6", "Joystick Button 6"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton7, LOCTEXT("JoystickButton7", "Joystick Button 7"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton8, LOCTEXT("JoystickButton8", "Joystick Button 8"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton9, LOCTEXT("JoystickButton9", "Joystick Button 9"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton10, LOCTEXT("JoystickButton10", "Joystick Button 10"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton11, LOCTEXT("JoystickButton11", "Joystick Button 11"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton12, LOCTEXT("JoystickButton12", "Joystick Button 12"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton13, LOCTEXT("JoystickButton13", "Joystick Button 13"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton14, LOCTEXT("JoystickButton14", "Joystick Button 14"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton15, LOCTEXT("JoystickButton15", "Joystick Button 15"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton16, LOCTEXT("JoystickButton16", "Joystick Button 16"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton17, LOCTEXT("JoystickButton17", "Joystick Button 17"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton18, LOCTEXT("JoystickButton18", "Joystick Button 18"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton19, LOCTEXT("JoystickButton19", "Joystick Button 19"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton20, LOCTEXT("JoystickButton20", "Joystick Button 20"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton21, LOCTEXT("JoystickButton21", "Joystick Button 21"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton22, LOCTEXT("JoystickButton22", "Joystick Button 22"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton23, LOCTEXT("JoystickButton23", "Joystick Button 23"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton24, LOCTEXT("JoystickButton24", "Joystick Button 24"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton25, LOCTEXT("JoystickButton25", "Joystick Button 25"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton26, LOCTEXT("JoystickButton26", "Joystick Button 26"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton27, LOCTEXT("JoystickButton27", "Joystick Button 27"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton28, LOCTEXT("JoystickButton28", "Joystick Button 28"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton29, LOCTEXT("JoystickButton29", "Joystick Button 29"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton30, LOCTEXT("JoystickButton30", "Joystick Button 30"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton31, LOCTEXT("JoystickButton31", "Joystick Button 31"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickButton32, LOCTEXT("JoystickButton32", "Joystick Button 32"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisX, LOCTEXT("JoystickAxisX", "Joystick Axis X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisY, LOCTEXT("JoystickAxisY", "Joystick Axis Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickAxisZ, LOCTEXT("JoystickAxisZ", "Joystick Axis Z"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisX, LOCTEXT("JoystickRAxisX", "Joystick RAxis X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisY, LOCTEXT("JoystickRAxisY", "Joystick RAxis Y"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickRAxisZ, LOCTEXT("JoystickRAxisZ", "Joystick RAxis Z"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV1X, LOCTEXT("JoystickPOV1X", "Joystick POV1 X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV1Y, LOCTEXT("JoystickPOV1Y", "Joystick POV1 Y"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV2X, LOCTEXT("JoystickPOV2X", "Joystick POV2 X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV2Y, LOCTEXT("JoystickPOV2Y", "Joystick POV2 Y"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV3X, LOCTEXT("JoystickPOV3X", "Joystick POV3 X"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickPOV3Y, LOCTEXT("JoystickPOV3Y", "Joystick POV3 Y"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickSlider1, LOCTEXT("JoystickSlider1", "Joystick Slider 1"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(EKeysJoystick::JoystickSlider2, LOCTEXT("JoystickSlider2", "Joystick Slider 2"), FKeyDetails::FloatAxis));

	if (S_OK == InitDirectInput()){
		UE_LOG(LogClass, Log, TEXT("Direct Input initialized."));
	}
	else{
		UE_LOG(LogClass, Log, TEXT("Direct Input initialization failed."));
	}

	m_pCollector = new DataCollector;

}

#undef LOCTEXT_NAMESPACE

void FJoystickPlugin::ShutdownModule()
{
	// TODO:cleanup any joystick function
	delete m_pCollector;
}


//Public API Implementation

/** Public API - Required **/

void FJoystickPlugin::SetDelegate(JoystickDelegate* newDelegate)
{
	joystickDelegate = newDelegate;
}

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
	//If we don't have a joystick check again maybe one was plugged in
	if (!g_pJoystick){
		if (S_OK != CheckForJoystickPlugin()){
			return;
		}
	}

	//get the freshest data
	joystickControllerDataUE* newJoyData = new joystickControllerDataUE;
	if(GetDeviceState(newJoyData))
		m_pCollector->UpdateData(newJoyData);
	delete newJoyData;

	//Ticks separately
	//Call the delegate once it has been set
	if (joystickDelegate != NULL)
	{
		DelegateTick(DeltaTime);
	}
	
}


void FJoystickPlugin::DelegateCheckEnabledCount(bool* plugNotChecked)
{
	if (!*plugNotChecked) return;

	*plugNotChecked = false;
}

void EmitInputMappingButtonPressed(int button)
{
	//currently supports buttons 1-16
	switch (button)
	{
	case 1:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton1, 0, 0);
		break;
	case 2:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton2, 0, 0);
		break;
	case 3:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton3, 0, 0);
		break;
	case 4:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton4, 0, 0);
		break;
	case 5:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton5, 0, 0);
		break;
	case 6:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton6, 0, 0);
		break;
	case 7:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton7, 0, 0);
		break;
	case 8:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton8, 0, 0);
		break;
	case 9:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton9, 0, 0);
		break;
	case 10:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton10, 0, 0);
		break;
	case 11:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton11, 0, 0);
		break;
	case 12:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton12, 0, 0);
		break;
	case 13:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton13, 0, 0);
		break;
	case 14:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton14, 0, 0);
		break;
	case 15:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton15, 0, 0);
		break;
	case 16:
		FSlateApplication::Get().OnControllerButtonPressed(EKeysJoystick::JoystickButton16, 0, 0);
		break;
	default:
		break;
	}
}

void EmitInputMappingButtonReleased(int button)
{
	switch (button)
	{
	case 1:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton1, 0, 0);
		break;
	case 2:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton2, 0, 0);
		break;
	case 3:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton3, 0, 0);
		break;
	case 4:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton4, 0, 0);
		break;
	case 5:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton5, 0, 0);
		break;
	case 6:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton6, 0, 0);
		break;
	case 7:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton7, 0, 0);
		break;
	case 8:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton8, 0, 0);
		break;
	case 9:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton9, 0, 0);
		break;
	case 10:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton10, 0, 0);
		break;
	case 11:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton11, 0, 0);
		break;
	case 12:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton12, 0, 0);
		break;
	case 13:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton13, 0, 0);
		break;
	case 14:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton14, 0, 0);
		break;
	case 15:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton15, 0, 0);
		break;
	case 16:
		FSlateApplication::Get().OnControllerButtonReleased(EKeysJoystick::JoystickButton16, 0, 0);
		break;
	default:
		break;
	}
}

/** Internal Tick - Called by the Plugin */
void FJoystickPlugin::DelegateTick(float DeltaTime)
{

	//joystickControllerDataUE* pJoyData;
	bool plugNotChecked = true;

	//Update delegate pointer
	joystickDelegate->_latestFrame = m_pCollector->currentController;

	UJoystickSingleController* current = m_pCollector->currentController;
	UJoystickSingleController* prev = m_pCollector->prevController;

	// check buttons 
	int bitVal = 0;
	for (int i = 0; i < 32; i++)
	{
		if (i == 0) bitVal = 1;
		else bitVal = pow(2, i);

		// Buttons 1 to 32
		if (false == ((current->ButtonsPressedLow & bitVal) == (prev->ButtonsPressedLow & bitVal))){
			// button state has changed
			if (current->ButtonsPressedLow & bitVal)
			{
				joystickDelegate->JoystickButtonPressed(i + 1, current);
				EmitInputMappingButtonPressed(i + 1);
			}
			else
			{
				joystickDelegate->JoystickButtonReleased(i + 1, current);
				EmitInputMappingButtonReleased(i + 1);
			}
		}

		// Buttons 33 to 128
		if (false == ((current->ButtonsPressedHigh & bitVal) == (prev->ButtonsPressedHigh& bitVal))){
			// button state has changed
			if (current->ButtonsPressedHigh & bitVal)
			{
				joystickDelegate->JoystickButtonPressed(i + 33, current);
				EmitInputMappingButtonPressed(i + 33);
			}
			else
			{
				joystickDelegate->JoystickButtonReleased(i + 33, current);
				EmitInputMappingButtonReleased(i + 33);
			}
		}
	}

	//check axis
	if (current->Axis != prev->Axis)
	{
		joystickDelegate->AxisChanged(current->Axis, current);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickAxisX, 0, current->Axis.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickAxisY, 0, current->Axis.Y);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickAxisZ, 0, current->Axis.Z);
	}

	//check raxis
	if (current->RAxis != prev->RAxis)
	{
		joystickDelegate->RAxisChanged(current->RAxis, current);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickRAxisX, 0, current->RAxis.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickRAxisY, 0, current->RAxis.Y);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickRAxisZ, 0, current->RAxis.Z);
	}

	//check pov
	if (current->POV0 != prev->POV0)
	{
		joystickDelegate->POVChanged(current->POV0, current);

		//Input Mapping, convert to joystick axis
		FVector2D povAxis = current->POVAxis(POV_1);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV1X, 0, povAxis.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV1Y, 0, povAxis.Y);
	}
	if (current->POV1 != prev->POV1)
	{
		joystickDelegate->POVChanged(current->POV1, current);

		//Input Mapping, convert to joystick axis
		FVector2D povAxis = current->POVAxis(POV_1);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV2X, 0, povAxis.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV2Y, 0, povAxis.Y);
	}
	if (current->POV2 != prev->POV2)
	{
		joystickDelegate->POVChanged(current->POV2, current);

		//Input Mapping, convert to joystick axis
		FVector2D povAxis = current->POVAxis(POV_3);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV3X, 0, povAxis.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickPOV3Y, 0, povAxis.Y);
	}

	//check slider
	if (current->Slider != prev->Slider)
	{
		joystickDelegate->SliderChanged(current->Slider, current);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickSlider1, 0, current->Slider.X);
		FSlateApplication::Get().OnControllerAnalog(EKeysJoystick::JoystickSlider2, 0, current->Slider.Y);
	}
}