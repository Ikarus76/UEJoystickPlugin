#include "JoystickPluginPrivatePCH.h"
#include "JoystickComponent.h"
#include "JoystickSingleController.h"
#include "JoystickInterface.h"
#include "Engine.h"
#include "CoreUObject.h"


UJoystickComponent::UJoystickComponent(const FObjectInitializer &init) : UActorComponent(init)
{
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;	//the component automatically ticks so we don't have to
}

void UJoystickComponent::OnRegister()
{
	Super::OnRegister();

	//Attach the delegate pointer automatically to the owner of the component
	ValidSelfPointer = this;
	SetInterfaceDelegate(GetOwner());
	JoystickStartup();
}

void UJoystickComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Forward the component tick
	JoystickTick(DeltaTime);
}

//Blueprint exposing the HydraDelegate Methods
//Override Callable Functions - Required to forward their implementations in order to compile, cannot skip implementation or bp definition
bool UJoystickComponent::IsAvailable()
{
	return JoystickDelegate::JoystickIsAvailable();
}


UJoystickSingleController* UJoystickComponent::GetLatestFrame()
{
	return JoystickDelegate::JoystickGetLatestFrame();
}


