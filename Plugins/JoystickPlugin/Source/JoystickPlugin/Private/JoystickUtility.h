#pragma once

DECLARE_LOG_CATEGORY_EXTERN(JoystickPluginLog, Log, All);

FVector JoystickUtilityNormalizeAxis(FVector in);
FVector2D JoystickUtilityNormalizeSlider(FVector2D value);
float JoystickUtilityNormalizeForceValue(int32 value);
int32 JoystickUtilityDeNormalizeForceValue(float value);