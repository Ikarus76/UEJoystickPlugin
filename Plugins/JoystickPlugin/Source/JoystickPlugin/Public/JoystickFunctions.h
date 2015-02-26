#include "JoystickInterface.h"

#include "JoystickFunctions.generated.h"

UCLASS(BlueprintType)
class UJoystickFunctions : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FVector2D POVAxis(EJoystickPOVDirection Direction);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
    static FJoystickInfo GetJoystick(int32 DeviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FJoystickState GetJoystickState(int32 DeviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FJoystickState GetPreviousJoystickState(int32 DeviceId);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static int32 JoystickCount();

	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	static void RegisterForJoystickEvents(UObject* Listener);

	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	static void MapJoystickDeviceToPlayer(int32 DeviceId, int32 Player);

	UFUNCTION(BlueprintCallable, Category = JoystickFunctions)
	static void IgnoreGameControllers(bool bIgnore);
};
