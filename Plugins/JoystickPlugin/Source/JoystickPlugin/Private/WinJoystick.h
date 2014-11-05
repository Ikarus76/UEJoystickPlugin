#pragma once

#if PLATFORM_WINDOWS

#define JOYSTICK_DEVICE_ENABLED 1

#define STRICT 
#define DIRECTINPUT_VERSION 0x0800 
#define _CRT_SECURE_NO_DEPRECATE 
#ifndef _WIN32_DCOM 
#define _WIN32_DCOM 
#endif 

#include "AllowWindowsPlatformTypes.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <windows.h> 
#include <stdio.h> 
#include <tchar.h> 
#include <commctrl.h> 
#include <basetsd.h> 

#pragma warning(push) 
#pragma warning(disable:6000 28251) 
#include <dinput.h> 
#pragma warning(pop) 

#include <dinputd.h> 
#include <assert.h> 
#include <oleauto.h> 
#include <shellapi.h> 
#include <wbemidl.h>

#include "FJoystickDevice.h"

namespace {

	struct XINPUT_DEVICE_NODE {
		DWORD dwVidPid;
		XINPUT_DEVICE_NODE* pNext;
	};

	XINPUT_DEVICE_NODE*     g_pXInputDeviceList = nullptr;
	LPDIRECTINPUT8          g_pDI = nullptr;
	LPDIRECTINPUTDEVICE8    g_pJoystick = nullptr;

	struct DI_ENUM_CONTEXT
	{
		DIJOYCONFIG* pPreferredJoyCfg;
		bool bPreferredJoyCfgValid;
	};


	/*
	struct FJoystickManager {
		XINPUT_DEVICE_NODE*     pXInputDeviceList = nullptr;
		LPDIRECTINPUT8          pDI = nullptr;
		LPDIRECTINPUTJOYCONFIG8 pJoyConfig = nullptr;
		DIJOYCONFIG				PreferredJoyCfg;
		bool					bFilterOutXinputDevices = false;

		struct {
			DIJOYCONFIG* pPreferredJoyCfg;
			bool bPreferredJoyCfgValid;
		} enumContext;
	};

	struct FJoystickHandler {
		FJoystickManager *Manager;
		FJoystickDeviceInfo &DeviceInfo;

		FORCEINLINE FJoystickHandler(FJoystickManager *InManager, FJoystickDeviceInfo &InDeviceInfo) :
			Manager(InManager),
			DeviceInfo(InDeviceInfo)
		{}
	};
	*/

	// forward declarations
	HRESULT SetupForIsXInputDevice();
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	void CleanupForIsXInputDevice();
	BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
	///////////////////////////


	HRESULT InitDirectInput()
	{
		HRESULT hr;

		// Register with the DirectInput subsystem and get a pointer
		// to a IDirectInput interface we can use.
		// Create a DInput object
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_pDI, nullptr)))
			return hr;


		//if (g_bFilterOutXinputDevices)
			SetupForIsXInputDevice();

		DIJOYCONFIG PreferredJoyCfg = { 0 };
		DI_ENUM_CONTEXT enumContext;
		enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
		enumContext.bPreferredJoyCfgValid = false;

		IDirectInputJoyConfig8* pJoyConfig = nullptr;
		if (FAILED(hr = g_pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
			return hr;

		PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
		if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE))) // This function is expected to fail if no joystick is attached
			enumContext.bPreferredJoyCfgValid = true;
		SAFE_RELEASE(pJoyConfig);

		// Look for a simple joystick we can use for this sample program.
		if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
			EnumJoysticksCallback,
			&enumContext, DIEDFL_ATTACHEDONLY)))
			return hr;

		//if (g_bFilterOutXinputDevices)
			CleanupForIsXInputDevice();

		// Make sure we got a joystick
		if (!g_pJoystick)
		{
			return S_FALSE;
		}

		// Set the data format to "simple joystick" - a predefined data format 
		//
		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if (FAILED(hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
			return hr;

		/*
		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.
		if (FAILED(hr = g_pJoystick->SetCooperativeLevel(hDlg, DISCL_EXCLUSIVE |
			DISCL_FOREGROUND)))
			return hr;*/

		// Enumerate the joystick objects. The callback function enabled user
		// interface elements for objects that are found, and sets the min/max
		// values property for discovered axes.
		if (FAILED(hr = g_pJoystick->EnumObjects(EnumObjectsCallback,
			NULL/*enum param!*/, DIDFT_ALL)))
			return hr;

		return S_OK;
	}


	/**
	* Enum each PNP device using WMI and check each device ID to see if it contains "IG_" (ex. "VID_045E&PID_028E&IG_00").
	* If it does, then it's an XInput device. Unfortunately this information can not be found by just using DirectInput.
	* Checking against a VID/PID of 0x028E/0x045E won't find 3rd party or future XInput devices.
	*
	* This function stores the list of xinput devices in a linked list at pXInputDeviceList, and IsXInputDevice() searchs that linked list.
	*/
	FORCEINLINE HRESULT SetupForIsXInputDevice()
	{
		IWbemServices* pIWbemServices = nullptr;
		IEnumWbemClassObject* pEnumDevices = nullptr;
		IWbemLocator* pIWbemLocator = nullptr;
		IWbemClassObject* pDevices[20] = { 0 };
		BSTR bstrDeviceID = nullptr;
		BSTR bstrClassName = nullptr;
		BSTR bstrNamespace = nullptr;
		DWORD uReturned = 0;
		bool bCleanupCOM = false;
		UINT iDevice = 0;
		VARIANT var;
		HRESULT hr;

		// CoInit if needed
		hr = CoInitialize(nullptr);
		bCleanupCOM = SUCCEEDED(hr);

		// Create WMI
		hr = CoCreateInstance(__uuidof(WbemLocator),
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWbemLocator),
			(LPVOID*)&pIWbemLocator);
		if (FAILED(hr) || pIWbemLocator == nullptr)
			goto LCleanup;

		// Create BSTRs for WMI
		bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if (bstrNamespace == nullptr) goto LCleanup;
		bstrDeviceID = SysAllocString(L"DeviceID");           if (bstrDeviceID == nullptr)  goto LCleanup;
		bstrClassName = SysAllocString(L"Win32_PNPEntity");    if (bstrClassName == nullptr) goto LCleanup;

		// Connect to WMI 
		hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0L,
			0L, nullptr, nullptr, &pIWbemServices);
		if (FAILED(hr) || pIWbemServices == nullptr)
			goto LCleanup;

		// Switch security level to IMPERSONATE
		(void)CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, 0);

		// Get list of Win32_PNPEntity devices
		hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
		if (FAILED(hr) || pEnumDevices == nullptr)
			goto LCleanup;

		// Loop over all devices
		for (;;)
		{
			// Get 20 at a time
			hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
			if (FAILED(hr))
				goto LCleanup;
			if (uReturned == 0)
				break;

			for (iDevice = 0; iDevice < uReturned; iDevice++)
			{
				if (!pDevices[iDevice])
					continue;

				// For each device, get its device ID
				hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);
				if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
				{
					// Check if the device ID contains "IG_".  If it does, then it’s an XInput device
					// Unfortunately this information can not be found by just using DirectInput 
					if (wcsstr(var.bstrVal, L"IG_"))
					{
						// If it does, then get the VID/PID from var.bstrVal
						DWORD dwPid = 0, dwVid = 0;
						WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
						if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
							dwVid = 0;
						WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
						if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
							dwPid = 0;

						DWORD dwVidPid = MAKELONG(dwVid, dwPid);

						// Add the VID/PID to a linked list
						XINPUT_DEVICE_NODE* pNewNode = new XINPUT_DEVICE_NODE;
						if (pNewNode)
						{
							pNewNode->dwVidPid = dwVidPid;
							pNewNode->pNext = g_pXInputDeviceList;
							g_pXInputDeviceList = pNewNode;
						}
					}
				}
				SAFE_RELEASE(pDevices[iDevice]);
			}
		}

	LCleanup:
		if (bstrNamespace)
			SysFreeString(bstrNamespace);
		if (bstrDeviceID)
			SysFreeString(bstrDeviceID);
		if (bstrClassName)
			SysFreeString(bstrClassName);
		for (iDevice = 0; iDevice < 20; iDevice++)
			SAFE_RELEASE(pDevices[iDevice]);
		SAFE_RELEASE(pEnumDevices);
		SAFE_RELEASE(pIWbemLocator);
		SAFE_RELEASE(pIWbemServices);

		return hr;
	}

	/**
	* Returns true if the DirectInput device is also an XInput device.
	* Call SetupForIsXInputDevice() before, and CleanupForIsXInputDevice() after
	*/
	FORCEINLINE bool IsXInputDevice(const GUID* pGuidProductFromDirectInput)
	{
		// Check each xinput device to see if this device's vid/pid matches
		XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
		while (pNode)
		{
			if (pNode->dwVidPid == pGuidProductFromDirectInput->Data1)
				return true;
			pNode = pNode->pNext;
		}

		return false;
	}

	/**
	* Cleanup needed for IsXInputDevice()
	*/
	FORCEINLINE void CleanupForIsXInputDevice() {
		// Cleanup linked list 
		XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
		while (pNode) {
			XINPUT_DEVICE_NODE* pDelete = pNode;
			pNode = pNode->pNext;
			delete pDelete;
		}
	}

	/**
	* Called once for each enumerated joystick.
	* If we find one, create a device interface on it so we can play with it.
	*/
	/*static*/ 
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
	{
		auto pEnumContext = reinterpret_cast<DI_ENUM_CONTEXT*>(pContext);
		HRESULT hr;

		if (/*g_bFilterOutXinputDevices*/ false && IsXInputDevice(&pdidInstance->guidProduct))
			return DIENUM_CONTINUE;

		// Skip anything other than the perferred joystick device as defined by the control panel.  
		// Instead you could store all the enumerated joysticks and let the user pick.
		if (pEnumContext->bPreferredJoyCfgValid &&
			!IsEqualGUID(pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance))
			return DIENUM_CONTINUE;

		// Obtain an interface to the enumerated joystick.
		hr = g_pDI->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, nullptr);

		// If it failed, then we can't use this joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
		if (FAILED(hr))
			return DIENUM_CONTINUE;

		// Stop enumeration. Note: we're just taking the first joystick we get. You
		// could store all the enumerated joysticks and let the user pick.
		return DIENUM_STOP;
	}

	/**
	* Callback function for enumerating objects (axes, buttons, POVs) on a joystick.
	* This function enables user interface elements for objects that are found to exist, and scales axes min/max values.
	*/
	/*static*/ 
	BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext) 
	{

		// For axes that are returned, set the DIPROP_RANGE property for the 
		// enumerated axis in order to scale min/max values. 

		//TODO : SET PROPERTIES

		
		if (pdidoi->dwType & DIDFT_AXIS) 
		{
			// Joystick has didft_axes
			OutputDebugString(_T("DIDFT_AXIS\n"));

			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis 

			// Get the range for the axis 
			HRESULT hr = g_pJoystick->GetProperty(DIPROP_RANGE, &diprg.diph);
			switch (hr) {
			case DIERR_INVALIDPARAM:
				//JD_CERR("Failed to set joystick property: Invalid parameter!");
				return DIENUM_STOP;
			case DIERR_NOTINITIALIZED:
				//JD_CERR("Failed to set joystick property: Not initialized!");
				return DIENUM_STOP;
			case DIERR_OBJECTNOTFOUND:
				//JD_CERR("Failed to set joystick property: Object not found!");
				return DIENUM_STOP;
			case DIERR_UNSUPPORTED:
				//JD_CERR("Failed to set joystick property: Unknown error code (%#x)!", hr);
				return DIENUM_STOP;
			}

			//DeviceInfo.AxisInfo.MinValue = diprg.lMin;
			//DeviceInfo.AxisInfo.MaxValue = diprg.lMax;
			
		}
		// Set the UI to reflect what objects the joystick supports
		if (pdidoi->guidType == GUID_XAxis)
		{
			// XAxis
			OutputDebugString(_T("X_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_YAxis)
		{
			// Y-Axis
			OutputDebugString(_T("Y_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_ZAxis)
		{
			// Z-Axis
			OutputDebugString(_T("Z_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_RxAxis)
		{
			// RxAxis
			OutputDebugString(_T("RX_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_RyAxis)
		{
			// RyAxis
			OutputDebugString(_T("RY_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_RzAxis)
		{
			// RzAxis
			OutputDebugString(_T("RZ_AXIS\n"));
		}
		if (pdidoi->guidType == GUID_Slider)
		{
			OutputDebugString(_T("Slider\n"));
		}
		if (pdidoi->guidType == GUID_POV)
		{
			OutputDebugString(_T("POV\n"));
		}
		if (pdidoi->guidType == GUID_Key)
		{
			OutputDebugString(_T("Key\n"));
		}
		if (pdidoi->guidType == GUID_Button)
		{
			OutputDebugString(_T("Button\n"));
		}
		return DIENUM_CONTINUE;
	}

	/*FORCEINLINE*/ BOOL GetDeviceState(joystickControllerDataUE* pJoyData) {

		DIJOYSTATE2 js;				// DInput joystick state  

		if (!g_pJoystick) return false;

		HRESULT hr = g_pJoystick->Poll();
		// Poll the device to read the current state 
		if (FAILED(hr))
		{
			// DInput is telling us that the input stream has been
			// interrupted. We aren't tracking any state between polls, so
			// we don't have any special reset that needs to be done. We
			// just re-acquire and try again.
			hr = g_pJoystick->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = g_pJoystick->Acquire();

			// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
			// may occur when the app is minimized or in the process of 
			// switching, so just try again later 
			return false;
		}

		// Get the input's device state 
		hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js);
		pJoyData->XAxis = js.lX;
		pJoyData->YAxis = js.lY;
		pJoyData->ZAxis = js.lZ;

		pJoyData->RXAxis = js.lRx;
		pJoyData->RYAxis = js.lRy;
		pJoyData->RZAxis = js.lRz;

		pJoyData->POV1 = js.rgdwPOV[0];
		pJoyData->Slider1 = js.rglSlider[0];

		pJoyData->buttonsPressed = 0;
		int bitVal = 0;
		for (int i = 0; i < 128; i++){
			if (js.rgbButtons[i] != 0)
			{
				if (i == 0) bitVal = 1;
				else bitVal = pow(2,i);
			}
			pJoyData->buttonsPressed |= bitVal;
		}

		if (hr!=S_OK) {
			switch (hr)
			{
			case DIERR_INPUTLOST:
				return false;
			case DIERR_INVALIDPARAM:
				return false;
			case DIERR_NOTACQUIRED:
				return false;
			case DIERR_NOTINITIALIZED:
				return false;
			case E_PENDING:
				return false;
			default:
				return false;
			}
			//JD_CERR("Failed to get joystick device state!");
			return false; // The device should have been acquired during the Poll() 
		}
		else return true;
	}
}




#include "HideWindowsPlatformTypes.h"

#endif