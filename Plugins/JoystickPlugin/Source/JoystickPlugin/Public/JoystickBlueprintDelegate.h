#pragma once

#include "JoystickDelegate.h"
#include "JoystickInterface.h"
#include "Core.h"
//#include "HydraBlueprintDelegate.generated.h"


class JoystickBlueprintDelegate : public JoystickDelegate
{
public:
	
	//Events which are forwarded to the interface class. NB: not forwarded to component bp
	void JoystickButtonPressed(int32 buttonNr) override;
	void JoystickButtonReleased(int32 buttonNr) override;
	void XAxisChanged(int32 Value) override;
	void YAxisChanged(int32 Value) override;
	void ZAxisChanged(int32 Value) override;
	void RXAxisChanged(int32 Value) override;
	void RYAxisChanged(int32 Value) override;
	void RZAxisChanged(int32 Value) override;
	void POV1Changed(int32 Value) override;
	void Slider1Changed(int32 Value) override;

	//Callable Blueprint functions
	/** Check if the hydra is available/plugged in.*/
	bool JoystickIsAvailable();

	//** Get the latest available data given in a single frame. Valid ControllerId is 0 or 1  */
	UJoystickSingleController* JoystickGetLatestFrame();

	//** Required Calls */
	virtual void JoystickStartup() override;				//Call this somewhere in an initializing state such as BeginPlay()
	virtual void JoystickTick(float DeltaTime) override;	//Call this every tick

	//If you want an alternate delegate, set it here
	void SetInterfaceDelegate(UObject* newDelegate);

protected:
	UObject* ValidSelfPointer;	//REQUIRED: has to be set before HydraStartup by a UObject subclass.
private:
	UJoystickSingleController* _latestFrame;
	UObject* _interfaceDelegate;
	bool implementsInterface();
};