#include "JoystickInterface.h"

#include "JoystickFunctions.generated.h"

UCLASS(BlueprintType)
class UJoystickFunctions : public UObject
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static bool ButtonPressed(FJoystickState state, int32 number);

	UFUNCTION(BlueprintPure, Category = JoystickFunctions)
	static FVector2D POVAxis(TEnumAsByte<JoystickPOVDirection> direction);
};
