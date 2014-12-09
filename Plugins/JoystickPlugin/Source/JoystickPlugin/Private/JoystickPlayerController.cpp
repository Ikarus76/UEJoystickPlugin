#include "JoystickPluginPrivatePCH.h"

#include "IJoystickPlugin.h"
#include "JoystickDelegate.h"
#include "JoystickPlayerController.h"

//Constructor/Initializer
AJoystickPlayerController::AJoystickPlayerController(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Blueprint exposing the HydraDelegate Methods
//Override Callable Functions - Required to forward their implementations in order to compile, cannot skip implementation or bp definition
bool AJoystickPlayerController::IsAvailable()
{
	return JoystickDelegate::JoystickIsAvailable();
}

UJoystickSingleController* AJoystickPlayerController::GetLatestFrame()
{
	return JoystickDelegate::JoystickGetLatestFrame();
}

//Required Overrides, forward startup and tick.
void AJoystickPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ValidSelfPointer = this;	//required from v0.7
	JoystickStartup();
}

void AJoystickPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	JoystickTick(DeltaTime);
}

