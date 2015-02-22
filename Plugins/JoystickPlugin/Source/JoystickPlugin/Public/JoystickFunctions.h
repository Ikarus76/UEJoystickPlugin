#include "JoystickPluginPrivatePCH.h"

#include "JoystickInterface.h"

#include "JoystickFunctions.generated.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

UCLASS(BlueprintType)
class UJoystickFunctions : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FVector2D POVAxis(TEnumAsByte<JoystickPOVDirection> direction);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
    static FJoystickInfo GetJoystick(int32 deviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FJoystickState GetLatestFrame(int32 deviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FJoystickState GetPreviousFrame(int32 deviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static int32 JoystickCount();

	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	static bool RegisterForEvents(UObject* listener);
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
