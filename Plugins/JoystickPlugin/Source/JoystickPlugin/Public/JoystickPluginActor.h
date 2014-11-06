#pragma once

#include "GameFramework/Actor.h"
#include "JoystickDelegate.h"
#include "JoystickPluginActor.generated.h"



/**
 * Placeable Actor Example that receives Hydra input,
 * override any delegate functions to receive notifications.
 * Calling super is not necessary.
 *
 * Copy implementations to receive same functionality in a different class.
 */

UCLASS()
class AJoystickPluginActor : public AActor, public JoystickDelegate, public IJoystickInterface //Multiple Inheritance, add JoystickDelegate inheritance to inherit all delegate functionality
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


