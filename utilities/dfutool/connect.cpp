//
// Connect to the DFU device.
//
#include <windows.h>
#include <setupapi.h>
#include <stdio.h>

char *dfu_find_path()
{
    static GUID guid = { 0x3fe809ab, 0xfb91, 0x4cb5, { 0xa6, 0x43, 0x69, 0x67, 0x0d, 0x52, 0x36, 0x6e } };

    HDEVINFO devinfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (devinfo == INVALID_HANDLE_VALUE) {
        printf("Cannot get devinfo!\n");
        return 0;
    }

    int index;
    char *path = 0;
    SP_INTERFACE_DEVICE_DATA iface;

    iface.cbSize = sizeof(iface);
    for (index=0; SetupDiEnumDeviceInterfaces(devinfo, NULL, &guid, index, &iface); ++index) {
        DWORD needed;

        SetupDiGetDeviceInterfaceDetail(devinfo, &iface, NULL, 0, &needed, NULL);

        PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)alloca(needed);
        detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
        SP_DEVINFO_DATA did = { sizeof(SP_DEVINFO_DATA) };

        if (!SetupDiGetDeviceInterfaceDetail(devinfo, &iface, detail, needed, NULL, &did)) {
            printf("Device [%d]: cannot get path!\n", index);
            continue;
        }
        printf("Device [%d]: path %s\n", index, detail->DevicePath);

        if (! path) {
            // Use the first DFU device.
            path = strdup(detail->DevicePath);
        }

        char product[253];
        if (!SetupDiGetDeviceRegistryProperty(devinfo, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)product, sizeof(product), NULL)) {
            printf("Device [%d]: cannot get product name!\n", index);
            continue;
        }
        printf("Device [%d]: product %s\n", index, product);
    }
    SetupDiDestroyDeviceInfoList(devinfo);

    printf("%d devices found.\n", index);
    if (index == 0) {
        printf("Turn on your DFU device!\n");
    }
    return path;
}

int main(int argc, char* argv[])
{
    char *path = dfu_find_path();
    if (! path) {
        return 0;
    }

    printf("Device: %s\n", path);

    HANDLE dev = CreateFile(path, GENERIC_WRITE | GENERIC_READ,
        0, NULL, OPEN_EXISTING, 0, NULL);
    if (! dev) {
        printf("%s: cannot open\n", path);
        return -1;
    }

    //TODO
    return 0;
}
