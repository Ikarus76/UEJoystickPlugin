#pragma once

#include "GameFramework/PlayerController.h"
#include "JoystickDelegate.h"
#include "JoystickPlayerController.generated.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

class UJoystickSingleController;

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

UCLASS()
class AJoystickPlayerController : public APlayerController, public JoystickDelegate, public IJoystickInterface
{
	GENERATED_UCLASS_BODY()

	//Callable Blueprint functions - Need to be defined for direct access
	/** Check if the hydra is available/plugged in.*/
	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	bool IsAvailable();

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	FJoystickInfo GetJoystick(int32 playerIndex);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	FJoystickState GetLatestFrame(int32 playerIndex);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	int32 JoystickCount();

	//Override Initialization and Tick to forward *required* hydra functions.
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
