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

	DataCollector()
	{
		currDataUE = new joystickControllerDataUE;
		ZeroMemory(currDataUE, sizeof(joystickControllerDataUE));
		prevDataUE = new joystickControllerDataUE;
		ZeroMemory(prevDataUE, sizeof(joystickControllerDataUE));
	}
	~DataCollector()
	{
		delete currDataUE;
		delete prevDataUE;
		/*
		delete allData;
		delete allDataUE;
		delete[] historicalDataUE;*/
	}

	void UpdateData(joystickControllerDataUE* newData)
	{
		memcpy(prevDataUE, currDataUE, sizeof(joystickControllerDataUE));
		memcpy(currDataUE, newData, sizeof(joystickControllerDataUE));
	}
};

//Init and Runtime
void FJoystickPlugin::StartupModule()
{
	UE_LOG(LogClass, Log, TEXT("Attempting to startup Joystick Module."));

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
	joystickDelegate->_joystickLatestData = m_pCollector->currDataUE;	//set the delegate latest data pointer
}

void FJoystickPlugin::JoystickTick(float DeltaTime)
{
	//get the freshest data

	joystickControllerDataUE* newJoyData = new joystickControllerDataUE;
	if(GetDeviceState(newJoyData))
		m_pCollector->UpdateData(newJoyData);
	delete newJoyData;


	/*
	if (GetDeviceState(m_pCollector->allDataUE))
	{
		if ( m_pCollector)
	}*/

	//Call the delegate once it has been set
	if (joystickDelegate != NULL)
	{
		DelegateTick(DeltaTime);
	}
	
}


/** Delegate Functions, called by plugin to keep data in sync and to emit the events.*/
void FJoystickPlugin::DelegateUpdateAllData()
{
	//NB: HydraHistoryData[0] = *HydraLatestData gets updated after the tick to take in integrated data
}

void FJoystickPlugin::DelegateCheckEnabledCount(bool* plugNotChecked)
{
	if (!*plugNotChecked) return;

	if (joystickDelegate->_joystickLatestData->enabled)
		joystickDelegate->JoystickIsAvailable();

	*plugNotChecked = false;
}

/** Internal Tick - Called by the Plugin */
void FJoystickPlugin::DelegateTick(float DeltaTime)
{
	//Update Data History
	DelegateUpdateAllData();

	//joystickControllerDataUE* pJoyData;
	bool plugNotChecked = true;

	// check buttons
	int bitVal = 0;
	for (int i = 0; i < 128; i++)
	{
		if (i == 0) bitVal = 1;
		else bitVal = pow(2, i);

		if (false == ((m_pCollector->currDataUE->buttonsPressed & bitVal) == (m_pCollector->prevDataUE->buttonsPressed & bitVal))){
			// button state has changed
			if (m_pCollector->currDataUE->buttonsPressed & bitVal)
				joystickDelegate->JoystickButtonPressed(i + 1);
			else
				joystickDelegate->JoystickButtonReleased(i + 1);
		}
	}

	//check x axis
	if (m_pCollector->currDataUE->XAxis != m_pCollector->prevDataUE->XAxis)
		joystickDelegate->XAxisChanged(m_pCollector->currDataUE->XAxis);

	//check y axis
	if (m_pCollector->currDataUE->YAxis != m_pCollector->prevDataUE->YAxis)
		joystickDelegate->YAxisChanged(m_pCollector->currDataUE->YAxis);

	//check z axis
	if (m_pCollector->currDataUE->ZAxis != m_pCollector->prevDataUE->ZAxis)
		joystickDelegate->ZAxisChanged(m_pCollector->currDataUE->ZAxis);

	//check rx axis
	if (m_pCollector->currDataUE->RXAxis != m_pCollector->prevDataUE->RXAxis)
		joystickDelegate->RXAxisChanged(m_pCollector->currDataUE->RXAxis);

	//check ry axis
	if (m_pCollector->currDataUE->RYAxis != m_pCollector->prevDataUE->RYAxis)
		joystickDelegate->RYAxisChanged(m_pCollector->currDataUE->RYAxis);

	//check rz axis
	if (m_pCollector->currDataUE->RZAxis != m_pCollector->prevDataUE->RZAxis)
		joystickDelegate->RZAxisChanged(m_pCollector->currDataUE->RZAxis);

	//check pov 1
	if (m_pCollector->currDataUE->POV1 != m_pCollector->prevDataUE->POV1)
		joystickDelegate->POV1Changed(m_pCollector->currDataUE->POV1);

	//check slider 1
	if (m_pCollector->currDataUE->Slider1 != m_pCollector->prevDataUE->Slider1)
		joystickDelegate->Slider1Changed(m_pCollector->currDataUE->Slider1);

}