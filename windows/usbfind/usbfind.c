#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include <stdio.h>

// This is the GUID for the USB device class
// (A5DCBF10-6530-11D2-901F-00C04FB951ED)
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
           	0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 
				0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);

//DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
//            0x36FC9E60L, 0xC465, 0x11CF, 0x80, 0x56, 
//			0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

void EnumRegKeys(HKEY hkey)
{
    BYTE name[1024];
	int i;

	for (i=0; ; i++) {
		DWORD nbytes = sizeof(name);
		LONG rc = RegEnumKeyEx(hkey, i, name, &nbytes, 
			NULL, NULL, NULL, NULL);
		if (rc == ERROR_NO_MORE_ITEMS)
			break;
		printf("key %s\n", name);
	}
}

void EnumRegValues(HKEY hkey)
{
    BYTE name[1024];
    BYTE data[1024];
	int i;

	for (i=0; ; i++) {
		DWORD name_size = sizeof(name);
		DWORD data_size = sizeof(data);
		DWORD type;
		LONG rc = RegEnumValue(hkey, i, name, &name_size, NULL, 
			&type, data, &data_size);
		if (rc == ERROR_NO_MORE_ITEMS)
			break;
		printf("value %s = %s\n", name, data);
	}
}

void DumpKey(HKEY hkey)
{
	EnumRegKeys(hkey);
	EnumRegValues(hkey);;
}

int main()
{
    // We will try to get device information set for all USB devices that have a
    // device interface and are currently present on the system (plugged in).
    HDEVINFO dev_set = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE,
        NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (dev_set == INVALID_HANDLE_VALUE) {
        printf("Cannot get list of USB devices!\n");
        exit(-1);
    }

    // Prepare to enumerate all device interfaces for the device information
    // set that we retrieved with SetupDiGetClassDevs(..)
    SP_DEVICE_INTERFACE_DATA dev_intf;
    dev_intf.cbSize = sizeof(dev_intf);

    // Next, we will keep calling this SetupDiEnumDeviceInterfaces(..) until this
    // function causes GetLastError() to return  ERROR_NO_MORE_ITEMS. With each
    // call the idx value needs to be incremented to retrieve the next
    // device interface information.
    DWORD idx;
    for (idx=0; ; idx++) {
        SetupDiEnumDeviceInterfaces(dev_set, NULL,
            &GUID_DEVINTERFACE_USB_DEVICE, idx, &dev_intf);

        if (GetLastError() == ERROR_NO_MORE_ITEMS)
            break;

        // Get the required buffer size.
        DWORD nbytes;
        SetupDiGetDeviceInterfaceDetail(dev_set, &dev_intf, NULL, 0,
            &nbytes, NULL);
        //printf("%u: nbytes = %u\n", (int)idx, (int)nbytes);

        // As a last step we will need to get some more details for each
        // of device interface information we are able to retrieve. This
        // device interface detail gives us the information we need to identify
        // the device (VID/PID), and decide if it's useful to us. It will also
        // provide a DEVINFO_DATA structure which we can use to know the serial
        // port name for a virtual com port.
		char buf[1024];
        PSP_DEVICE_INTERFACE_DETAIL_DATA intf_detail = (void*)buf;
        intf_detail->cbSize = sizeof(*intf_detail);

        SP_DEVINFO_DATA dev_instance;
        dev_instance.cbSize = sizeof(dev_instance);

        if (! SetupDiGetDeviceInterfaceDetail(dev_set, &dev_intf,
                intf_detail, nbytes, &nbytes, &dev_instance))
        {
            printf("Cannot get device interface details!\n");
            continue;
        }

        printf("\n(%u)\n%s\n", (int)idx, intf_detail->DevicePath);

        // Check the registry for the port name

        HKEY hKey = SetupDiOpenDevRegKey(dev_set, &dev_instance,
            DICS_FLAG_GLOBAL, 0, DIREG_DRV, KEY_READ);
		DumpKey(hKey);
#if 0
        BYTE port_name[1024];
        nbytes = sizeof(port_name);
        DWORD dwType = REG_SZ;
        RegQueryValueEx(hKey, _T("DriverDesc"), NULL, &dwType, port_name, &nbytes);
        printf("Port name = '%s'\n", port_name);
#endif
        RegCloseKey(hKey);
    }
    SetupDiDestroyDeviceInfoList(dev_set);

    return 0;
}
