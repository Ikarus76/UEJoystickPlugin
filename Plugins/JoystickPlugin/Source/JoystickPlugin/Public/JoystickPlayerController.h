#pragma once

#include "GameFramework/PlayerController.h"
#include "JoystickDelegate.h"
#include "JoystickPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AJoystickPlayerController : public APlayerController, public JoystickDelegate, public IJoystickInterface
{
	GENERATED_UCLASS_BODY()

	//Callable Blueprint functions - Need to be defined for direct access
	/** Check if the hydra is available/plugged in.*/
	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	bool IsAvailable();

	//** Get the latest available data given in a single frame. Valid ControllerId is 0 or 1  */
	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	UJoystickSingleController* GetLatestFrame();

	//Override Initialization and Tick to forward *required* hydra functions.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
