#pragma once

#include "JoystickPluginPrivatePCH.h"
#include "JoystickDelegate.h"
#include "JoystickComponent.generated.h"

UCLASS(ClassGroup="Input Controller", meta=(BlueprintSpawnableComponent))
class UJoystickComponent : public UActorComponent, public JoystickDelegate
{
	GENERATED_UCLASS_BODY()

public:

	//Callable Blueprint functions - Need to be defined for direct access
	/** Check if the hydra is available/plugged in.*/
	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	bool IsAvailable();

	//** Get the latest available data given in a single frame. Valid ControllerId is 0 or 1  */
	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	UJoystickSingleController* GetLatestFrame();

	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
};