#include "JoystickPluginPrivatePCH.h"

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "JoystickDelegate.h"
#include "JoystickPluginActor.h"

//Constructor/Initializer
AJoystickPluginActor::AJoystickPluginActor(const FObjectInitializer& PCIP)
: Super(PCIP)
{
	PrimaryActorTick.bCanEverTick = true;
}

//Blueprint exposing the HydraDelegate Methods
//Override Callable Functions - Required to forward their implementations in order to compile, cannot skip implementation or bp definition
bool AJoystickPluginActor::IsAvailable()
{
	return JoystickDelegate::JoystickIsAvailable();
}

UJoystickSingleController* AJoystickPluginActor::GetLatestFrame()
{
	return JoystickDelegate::JoystickGetLatestFrame();
}

//Required Overrides
void AJoystickPluginActor::BeginPlay()
{
	Super::BeginPlay();

	//Actors by default aren't attached to the input chain, so we enable input for this actor to forward Key and Gamepad Events
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	EnableInput(PC);

	//Required Hydra Initialization
	ValidSelfPointer = this;	//required from v0.7
	JoystickStartup();
}

void AJoystickPluginActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Required Joystick Tick
	JoystickTick(DeltaTime);
}

