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
#include <dbt.h>
#include <sstream>
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
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

	struct XINPUT_DEVICE_NODE {
		DWORD dwVidPid;
		XINPUT_DEVICE_NODE* pNext;
	};

	XINPUT_DEVICE_NODE*     g_pXInputDeviceList = nullptr;
	LPDIRECTINPUT8          g_pDI = nullptr;
	LPDIRECTINPUTDEVICE8    g_pJoystick = nullptr;
	bool JoystickFound = false;

	//FF
	HWND					g_hWndFF;
	LPDIRECTINPUT8          g_pDIFF = nullptr;
	LPDIRECTINPUTDEVICE8    g_pJoystickFF = nullptr;
	LPDIRECTINPUTEFFECT     g_pEffect = nullptr;
	DWORD                   g_dwNumForceFeedbackAxis = 0;
	INT                     g_nXForce;
	INT                     g_nYForce;
	bool JoystickFFFound = false;

	//Hot Plugging, dummy window
	JoystickHotPlugInterface* hpDelegate;
	bool JoystickStatePluggedIn = false;
	HWND HPWindow;
	
	struct DI_ENUM_CONTEXT
	{
		DIJOYCONFIG* pPreferredJoyCfg;
		bool bPreferredJoyCfgValid;
	};

	DI_ENUM_CONTEXT enumContext;

	// forward declarations
	HRESULT SetupForIsXInputDevice();
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	void CleanupForIsXInputDevice();
	BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
	///////////////////////////

	/**
	* Utility
	*/

	BOOL is_main_window(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	}

	struct handle_data {
		unsigned long process_id;
		HWND best_handle;
	};

	BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		unsigned long process_id = 0;
		GetWindowThreadProcessId(handle, &process_id);
		if (data.process_id != process_id || !is_main_window(handle)) {
			return TRUE;
		}
		data.best_handle = handle;
		return FALSE;
	}

	HWND find_main_window(unsigned long process_id)
	{
		handle_data data;
		data.process_id = process_id;
		data.best_handle = 0;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.best_handle;
	}

	//Convenience
	HWND getProcessWindow(){
		DWORD processId = GetCurrentProcessId();
		return find_main_window(processId);
	}

	//Utility function used to debug address allocation
	void UtilityDebugAddress(void* pointer)
	{
		const void * address = static_cast<const void*>(pointer);
		std::stringstream ss;
		ss << address;
		std::string name = ss.str();
		FString string(name.c_str());

		UE_LOG(JoystickPluginLog, Log, TEXT("Address: %s"), *string);
	}

	/**
	*	Joystick Force Feedback
	*/

	BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst,
		VOID* pContext)
	{
		LPDIRECTINPUTDEVICE8 pDevice;
		HRESULT hr;

		// Obtain an interface to the enumerated force feedback device.
		hr = g_pDI->CreateDevice(pInst->guidInstance, &pDevice, nullptr);

		// If it failed, then we can't use this device for some
		// bizarre reason.  (Maybe the user unplugged it while we
		// were in the middle of enumerating it.)  So continue enumerating
		if (FAILED(hr))
			return DIENUM_CONTINUE;

		// We successfully created an IDirectInputDevice8.  So stop looking 
		// for another one.
		g_pJoystick = pDevice;

		JoystickFFFound = true;

		return DIENUM_STOP;
	}

	BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
		VOID* pContext)
	{
		auto pdwNumForceFeedbackAxis = reinterpret_cast<DWORD*>(pContext);

		if ((pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0)
			(*pdwNumForceFeedbackAxis)++;

		return DIENUM_CONTINUE;
	}

	void CleanupFF()
	{
		if (g_pJoystickFF)
			g_pJoystickFF->Unacquire();

		// Release any DirectInput objects.
		SAFE_RELEASE(g_pEffect);
		SAFE_RELEASE(g_pJoystickFF);
		SAFE_RELEASE(g_pDIFF);
	}

	HRESULT InitDirectInputFF()
	{
		UE_LOG(JoystickPluginLog, Warning, TEXT("Initializing force feedback bind."));

		DIPROPDWORD dipdw;
		HRESULT hr;

		if (!g_pJoystickFF)
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: No FF device found."));
			return S_OK;
		}

		// This tells DirectInput that we will be passing a DIJOYSTATE structure to
		// IDirectInputDevice8::GetDeviceState(). Even though we won't actually do
		// it in this sample. But setting the data format is important so that the
		// DIJOFS_* values work properly.
		if (FAILED(hr = g_pJoystickFF->SetDataFormat(&c_dfDIJoystick)))
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Set Data Format failed."));
			return hr;
		}

		g_hWndFF = GetForegroundWindow();
		//g_hWndFF = HPWindow;

		int length = ::GetWindowTextLength(g_hWndFF);
		TCHAR* buffer;
		buffer = new TCHAR[length + 1];
		memset(buffer, 0, (length + 1) * sizeof(TCHAR));

		GetWindowText(g_hWndFF, buffer, length + 1);

		FString returnText = FString(length, buffer);

		UE_LOG(JoystickPluginLog, Log, TEXT("Foreground window name: %s"), *returnText);

		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.
		// Exclusive access is required in order to perform force feedback.
		if (FAILED(hr = g_pJoystickFF->SetCooperativeLevel(g_hWndFF,
			DISCL_EXCLUSIVE |
			DISCL_BACKGROUND)))
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Set Cooperative Level failed."));
			return hr;
		}

		// Since we will be playing force feedback effects, we should disable the
		// auto-centering spring.
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = FALSE;

		if (FAILED(hr = g_pJoystickFF->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)))
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Set Auto-center Property failed."));
			return hr;
		}

		// Enumerate and count the axes of the joystick 
		if (FAILED(hr = g_pJoystickFF->EnumObjects(EnumAxesCallback,
			(VOID*)&g_dwNumForceFeedbackAxis, DIDFT_AXIS)))
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Enum ForceFeedback axis failed."));
			return hr;
		}

		//g_pJoystick = g_pJoystickFF;

		// This simple sample only supports one or two axis joysticks
		if (g_dwNumForceFeedbackAxis > 2)
			g_dwNumForceFeedbackAxis = 2;

		// This application needs only one effect: Applying raw forces.
		DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
		LONG rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf = { 0 };

		DIEFFECT eff;
		ZeroMemory(&eff, sizeof(eff));
		eff.dwSize = sizeof(DIEFFECT);
		eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration = INFINITE;
		eff.dwSamplePeriod = 0;
		eff.dwGain = DI_FFNOMINALMAX;
		eff.dwTriggerButton = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes = g_dwNumForceFeedbackAxis;
		eff.rgdwAxes = rgdwAxes;
		eff.rglDirection = rglDirection;
		eff.lpEnvelope = 0;
		eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
		eff.dwStartDelay = 0;

		// Create the prepared effect
		if (FAILED(hr = g_pJoystickFF->CreateEffect(GUID_ConstantForce,
			&eff, &g_pEffect, nullptr)))
		{
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Create Effect failed."));
			return hr;
		}

		if (!g_pEffect){
			UE_LOG(JoystickPluginLog, Warning, TEXT("DirectInput ForceFeedback Warning: Effect acquisition failed."));
			return E_FAIL;
		}

		if (g_pJoystickFF && g_hWndFF != nullptr){
			//g_pJoystickFF->Acquire();
			//if (g_pEffect)
				//g_pEffect->Start(1, 0);
		}

		UE_LOG(JoystickPluginLog, Log, TEXT("ForceFeedback Device found and initialized."));
		return S_OK;
	}

	HRESULT SetForceFeedbackXY(INT directionX, INT directionY, float magnitude)
	{
		//Shortcut it
		if (!g_pJoystickFF || !g_pEffect){
			UtilityDebugAddress(g_pJoystickFF);
			UtilityDebugAddress(g_pEffect);
			UE_LOG(JoystickPluginLog, Log, TEXT("No FF joystick or Effect, returning."));
			return S_OK;
		}

		g_nXForce = directionX * magnitude;
		g_nYForce = directionY * magnitude;

		// Modifying an effect is basically the same as creating a new one, except
		// you need only specify the parameters you are modifying
		LONG rglDirection[2] = { 0, 0 };

		DICONSTANTFORCE cf;

		if (g_dwNumForceFeedbackAxis == 1)
		{
			// If only one force feedback axis, then apply only one direction and 
			// keep the direction at zero
			cf.lMagnitude = g_nXForce;
			rglDirection[0] = 0;
		}
		else
		{
			// If two force feedback axis, then apply magnitude from both directions 
			rglDirection[0] = g_nXForce;
			rglDirection[1] = g_nYForce;
			cf.lMagnitude = (DWORD)sqrt((double)g_nXForce * (double)g_nXForce +
				(double)g_nYForce * (double)g_nYForce);
		}

		//Debug override
		rglDirection[0] = 0;
		rglDirection[1] = 0;
		//cf.lMagnitude = (DWORD)7000;
		cf.lMagnitude = DI_FFNOMINALMAX;

		DIEFFECT eff;
		ZeroMemory(&eff, sizeof(eff));
		eff.dwSize = sizeof(DIEFFECT);
		eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes = g_dwNumForceFeedbackAxis;
		eff.rglDirection = rglDirection;
		eff.lpEnvelope = 0;
		eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
		eff.dwStartDelay = 0;

		UtilityDebugAddress(g_pEffect);

		// Now set the new parameters and start the effect immediately.
		if (FAILED(g_pEffect->SetParameters(&eff, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS | DIEP_START)))
		{
			UE_LOG(JoystickPluginLog, Log, TEXT("Failed setting effects parameters!"));
		}

		UE_LOG(JoystickPluginLog, Log, TEXT("Force should be felt by now."));

		return S_OK;
	}

	/**
	*	HOT PLUGGING
	*/
	void CleanupHotPlugging()
	{
		DestroyWindow(HPWindow);
	}

	HRESULT CheckForJoystickPlugin()
	{
		HRESULT hr;

		//Release any references to the current joystick so we can pick up a different joystick if need be
		SAFE_RELEASE(g_pJoystick);
		//if (g_pJoystickFF)
		//	CleanupFF();

		JoystickFound = JoystickFFFound = false;
		//try finding a FF joystick - this works, but it causes a crash downstream on a ff device sadly. Commented out for now, can be removed if deemed appropriate
		/*if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
			EnumFFDevicesCallback, nullptr,
			DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK)))
		{
			UE_LOG(JoystickPluginLog, Log, TEXT("No Joystick FF Enumerate Devices found."));
			return hr;
		}

		if (JoystickFFFound){
			UE_LOG(JoystickPluginLog, Log, TEXT("FF Device found, starting FF specific init..."));
			JoystickFound = JoystickFFFound;
			g_pJoystickFF = g_pJoystick;
			InitDirectInputFF();
		}
		else{
		*/	UE_LOG(JoystickPluginLog, Log, TEXT("No FF Device found, grabbing regular joystick."));

			// Look for a simple joystick we can use for this sample program.
			if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
				EnumJoysticksCallback,
				&enumContext, DIEDFL_ATTACHEDONLY)))
			{
				UE_LOG(JoystickPluginLog, Log, TEXT("No Joystick Enumerate Devices found."));
				return hr;
			}
		//}

		CleanupForIsXInputDevice();

		// Make sure we got a joystick
		if (!g_pJoystick)
		{
			if (!JoystickFound && JoystickStatePluggedIn)
			{
				if (hpDelegate)
					hpDelegate->JoystickUnplugged();
				JoystickStatePluggedIn = false;
			}
			UE_LOG(JoystickPluginLog, Log, TEXT("No Joystick Device found."));
			return S_FALSE;
		}

		// Set the data format to "simple joystick" - a predefined data format 
		//
		// A data format specifies which controls on a device we are interested in,
		// and how they should be reported. This tells DInput that we will be
		// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
		if (FAILED(hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick2)))
			return hr;


		// Enumerate the joystick objects. The callback function enabled user
		// interface elements for objects that are found, and sets the min/max
		// values property for discovered axes.
		if (FAILED(hr = g_pJoystick->EnumObjects(EnumObjectsCallback,
			NULL/*enum param!*/, DIDFT_ALL)))
			return hr;

		//Debug
		UE_LOG(JoystickPluginLog, Log, TEXT("Hot Plug Joystick detected: %d state: %d"), JoystickFound, JoystickStatePluggedIn);
		
		//Detect
		if (JoystickFound && !JoystickStatePluggedIn)
		{
			if (hpDelegate)
				hpDelegate->JoystickPluggedIn();
			JoystickStatePluggedIn = true;
		}

		//Acquire the joystick
		return g_pJoystick->Acquire();

		//return S_OK;
	}

	INT_PTR WINAPI WinProcCallback(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam)
	{
		switch (message){

		case WM_DEVICECHANGE:
			CheckForJoystickPlugin();
			break;
		default:
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void EnableHotPlugListener()
	{
		//Make a dummy window, this is the only way I know to register for window events (successfully listening to WM_DEVICECHANGE messages)
		LPTSTR HPClassName = TEXT("HPClass");
		WNDCLASS HPClass;
		HWND HPWindow;

		HPClass.style = CS_VREDRAW;
		HPClass.lpfnWndProc = &WinProcCallback;
		HPClass.cbClsExtra = 0;
		HPClass.cbWndExtra = 0;
		HPClass.hInstance = hInstance;
		HPClass.hIcon = NULL;
		HPClass.hCursor = NULL;
		HPClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
		HPClass.lpszMenuName = NULL;
		HPClass.lpszClassName = HPClassName;
		if (!RegisterClass(&HPClass)) return;

		HPWindow = CreateWindow(HPClassName, NULL, WS_MINIMIZE, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

		if (HPWindow == NULL) return;

		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

		NotificationFilter.dbcc_size = sizeof(NotificationFilter);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_reserved = 0;

		NotificationFilter.dbcc_classguid = { 0x25dbce51, 0x6c8f, 0x4a72,
			0x8a, 0x6d, 0xb5, 0x4c, 0x2b, 0x4f, 0xc8, 0x35 };

		HDEVNOTIFY hDevNotify = RegisterDeviceNotification(NULL, &NotificationFilter, DEVICE_NOTIFY_SERVICE_HANDLE);

		UE_LOG(JoystickPluginLog, Log, TEXT("Registered for hot plugging."));
	}

	/**
	*	Joystick - Basics
	*/

	HRESULT InitDirectInput()
	{
		HRESULT hr;

		// Register with the DirectInput subsystem and get a pointer
		// to a IDirectInput interface we can use.
		// Create a DInput object
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_pDI, nullptr)))
			return hr;

		DIJOYCONFIG PreferredJoyCfg = { 0 };
		enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
		enumContext.bPreferredJoyCfgValid = false;

		IDirectInputJoyConfig8* pJoyConfig = nullptr;
		if (FAILED(hr = g_pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
			return hr;

		PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);
		if (SUCCEEDED(pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE))) // This function is expected to fail if no joystick is attached
			enumContext.bPreferredJoyCfgValid = true;
		SAFE_RELEASE(pJoyConfig);

		CheckForJoystickPlugin();

		if (JoystickFound)
		{
			UE_LOG(JoystickPluginLog, Log, TEXT("Joystick found plugged in."));
			JoystickStatePluggedIn = true;
		}

		UE_LOG(JoystickPluginLog, Log, TEXT("Joystick Init success."));
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
						if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
							dwVid = 0;
						WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
						if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
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
		JoystickFound = true;

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

	BOOL GetDeviceState(joystickControllerDataUE* pJoyData) {

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
		pJoyData->Axis = FVector(js.lX, js.lY, js.lZ);
		pJoyData->RAxis = FVector(js.lRx, js.lRy, js.lRz);

		pJoyData->POV.X = js.rgdwPOV[0];
		pJoyData->POV.Y = js.rgdwPOV[1];
		pJoyData->POV.Z = js.rgdwPOV[2];

		pJoyData->Slider.X = js.rglSlider[0];
		pJoyData->Slider.Y = js.rglSlider[1];

		pJoyData->buttonsPressedL = 0;
		int bitVal = 0;
		for (int i = 0; i < 32; i++){
			if (js.rgbButtons[i] != 0)
			{
				if (i == 0) bitVal = 1;
				else bitVal = pow(2, i);
			}
			pJoyData->buttonsPressedL |= bitVal;
		}

		pJoyData->buttonsPressedH = 0;
		bitVal = 0;
		for (int i = 0; i < 32; i++){
			if (js.rgbButtons[i+32] != 0)
			{
				if (i == 0) bitVal = 1;
				else bitVal = pow(2, i);
			}
			pJoyData->buttonsPressedH |= bitVal;
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