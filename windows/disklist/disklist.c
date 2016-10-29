#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include <stdio.h>
#include <initguid.h>

// This is the GUID for the USB device class
// (A5DCBF10-6530-11D2-901F-00C04FB951ED)
DEFINE_GUID(GUID_DEVINTERFACE_DISK,                   
			0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 
			0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
				
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
    HDEVINFO dev_set = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK,
        NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
//    HDEVINFO dev_set = SetupDiGetClassDevs(0, "USBSTOR", 0, 
//		DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (dev_set == INVALID_HANDLE_VALUE) {
        printf("Cannot get list of disks!\n");
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
            &GUID_DEVINTERFACE_DISK, idx, &dev_intf);

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

#if 0
//#include <Ntddstor.h>

//#pragma comment( lib, "setupapi.lib" )

#include <iostream>
#include <string>

#define START_ERROR_CHK()           \
    DWORD error = ERROR_SUCCESS;    \
    DWORD failedLine;               \
    string failedApi;

#define CHK( expr, api )            \
    if ( !( expr ) ) {              \
        error = GetLastError( );    \
        failedLine = __LINE__;      \
        failedApi = ( api );        \
        goto Error_Exit;            \
    }

#define END_ERROR_CHK()             \
    error = ERROR_SUCCESS;          \
    Error_Exit:                     \
    if ( ERROR_SUCCESS != error ) { \
        cout << failedApi << " failed at " << failedLine << " : Error Code - " << error << endl;    \
    }

int main( int argc, char **argv ) {

    HDEVINFO diskClassDevices;
    GUID diskClassDeviceInterfaceGuid = GUID_DEVINTERFACE_DISK;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData;
    DWORD requiredSize;
    DWORD deviceIndex;

    HANDLE disk = INVALID_HANDLE_VALUE;
    STORAGE_DEVICE_NUMBER diskNumber;
    DWORD bytesReturned;

    START_ERROR_CHK();

    //
    // Get the handle to the device information set for installed
    // disk class devices. Returns only devices that are currently
    // present in the system and have an enabled disk device
    // interface.
    //
    diskClassDevices = SetupDiGetClassDevs( &diskClassDeviceInterfaceGuid,
                                            NULL,
                                            NULL,
                                            DIGCF_PRESENT |
                                            DIGCF_DEVICEINTERFACE );
    CHK( INVALID_HANDLE_VALUE != diskClassDevices,
         "SetupDiGetClassDevs" );

    ZeroMemory( &deviceInterfaceData, sizeof( SP_DEVICE_INTERFACE_DATA ) );
    deviceInterfaceData.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA );
    deviceIndex = 0;

    while ( SetupDiEnumDeviceInterfaces( diskClassDevices,
                                         NULL,
                                         &diskClassDeviceInterfaceGuid,
                                         deviceIndex,
                                         &deviceInterfaceData ) ) {

        ++deviceIndex;

        SetupDiGetDeviceInterfaceDetail( diskClassDevices,
                                         &deviceInterfaceData,
                                         NULL,
                                         0,
                                         &requiredSize,
                                         NULL );
        CHK( ERROR_INSUFFICIENT_BUFFER == GetLastError( ),
             "SetupDiGetDeviceInterfaceDetail - 1" );

        deviceInterfaceDetailData = ( PSP_DEVICE_INTERFACE_DETAIL_DATA ) malloc( requiredSize );
        CHK( NULL != deviceInterfaceDetailData,
             "malloc" );

        ZeroMemory( deviceInterfaceDetailData, requiredSize );
        deviceInterfaceDetailData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );

        CHK( SetupDiGetDeviceInterfaceDetail( diskClassDevices,
                                              &deviceInterfaceData,
                                              deviceInterfaceDetailData,
                                              requiredSize,
                                              NULL,
                                              NULL ),
             "SetupDiGetDeviceInterfaceDetail - 2" );

        disk = CreateFile( deviceInterfaceDetailData->DevicePath,
                           GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL );
        CHK( INVALID_HANDLE_VALUE != disk,
             "CreateFile" );

        CHK( DeviceIoControl( disk,
                              IOCTL_STORAGE_GET_DEVICE_NUMBER,
                              NULL,
                              0,
                              &diskNumber,
                              sizeof( STORAGE_DEVICE_NUMBER ),
                              &bytesReturned,
                              NULL ),
             "IOCTL_STORAGE_GET_DEVICE_NUMBER" );

        CloseHandle( disk );
        disk = INVALID_HANDLE_VALUE;

        cout << deviceInterfaceDetailData->DevicePath << endl;
        cout << "\\\\?\\PhysicalDrive" << diskNumber.DeviceNumber << endl;
        cout << endl;
    }
    CHK( ERROR_NO_MORE_ITEMS == GetLastError( ),
         "SetupDiEnumDeviceInterfaces" );

    END_ERROR_CHK();

Exit:

    if ( INVALID_HANDLE_VALUE != diskClassDevices ) {
        SetupDiDestroyDeviceInfoList( diskClassDevices );
    }

    if ( INVALID_HANDLE_VALUE != disk ) {
        CloseHandle( disk );
    }

    return error;
}
#endif