#include <windows.h>
#include <winioctl.h>
//#include <tchar.h>
//#include <setupapi.h>
//#include <initguid.h>
#include <stdio.h>

typedef struct _DEVICE_NUMBER {
    DEVICE_TYPE  DeviceType;
    ULONG  DeviceNumber;
    ULONG  PartitionNumber;
} DEVICE_NUMBER, *PDEVICE_NUMBER;

static int is_accessible(HANDLE h, char *drive_name)
{
    // ensure that the drive is actually accessible
    // multi-card hubs were reporting "removable" even when empty
    DWORD cbBytesReturned;
    if (! DeviceIoControl(h, IOCTL_STORAGE_CHECK_VERIFY2,
            NULL, 0, NULL, 0, &cbBytesReturned, NULL))
    {
        // IOCTL_STORAGE_CHECK_VERIFY2 fails on some devices under XP/Vista.
        // Try the other (slower) method, just in case.
        HANDLE g = CreateFile(drive_name, FILE_READ_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);
        if (g == INVALID_HANDLE_VALUE)
            return 0;

        if (! DeviceIoControl(g, IOCTL_STORAGE_CHECK_VERIFY,
                NULL, 0, NULL, 0, &cbBytesReturned, NULL)) {
            CloseHandle(g);
            return 0;
        }
        CloseHandle(g);
    }
    return 1;
}

static unsigned disk_kbytes(HANDLE h)
{
    DISK_GEOMETRY_EX geom;
    DWORD junk;

    if (! DeviceIoControl(h, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0,
            &geom, sizeof(geom), &junk, NULL))
        return 0;

    return (unsigned long long)geom.DiskSize.QuadPart / 1024;
}

int main()
{
    // GetLogicalDrives returns 0 on failure, or a bitmask representing
    // the drives available on the system (bit 0 = A:, bit 1 = B:, etc)
    unsigned drive_mask = GetLogicalDrives();
    int i;

    for (i = 0; drive_mask != 0; i++, drive_mask >>= 1) {
        if (drive_mask & 1) {
            char drive_char = 'A' + i;
            char drive_name[] = "\\\\.\\A:\\";
            drive_name[4] = drive_char;

            int drive_type = GetDriveType(drive_name);
            if (drive_type != DRIVE_REMOVABLE)
                continue;

            drive_name[6] = 0;
            HANDLE h = CreateFile(drive_name, FILE_READ_ATTRIBUTES,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                OPEN_EXISTING, 0, NULL);
            if (h == INVALID_HANDLE_VALUE)
                continue;

            if (! is_accessible(h, drive_name)) {
                CloseHandle(h);
                continue;
            }

            char buf[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
            PSTORAGE_DEVICE_DESCRIPTOR sd = (void*) buf;
            sd->Size = sizeof(buf);

            // get the device number if the drive is
            // removable or (fixed AND on the usb bus, SD, or MMC (undefined in XP/mingw))
            DWORD dwOutBytes;
            STORAGE_PROPERTY_QUERY query;
            query.PropertyId = StorageDeviceProperty;
            query.QueryType = PropertyStandardQuery;
            if (! DeviceIoControl(h, IOCTL_STORAGE_QUERY_PROPERTY,
                                &query, sizeof(STORAGE_PROPERTY_QUERY), sd,
                                sd->Size, &dwOutBytes, NULL)) {
                CloseHandle(h);
                continue;
            }
            if (sd->BusType != BusTypeUsb) {
                CloseHandle(h);
                continue;
            }

            DEVICE_NUMBER dev_num;
            if (! DeviceIoControl(h, IOCTL_STORAGE_GET_DEVICE_NUMBER,
                                  NULL, 0, &dev_num, sizeof(DEVICE_NUMBER),
                                  &dwOutBytes, NULL)) {
                CloseHandle(h);
                continue;
            }

            unsigned kbytes = disk_kbytes(h);

            printf("\n");
            printf("%c: Removable USB storage, %u kbytes\n", drive_char, kbytes);
            printf("Device number = %u\n", (int)dev_num.DeviceNumber);

            CloseHandle(h);
        }
    }
    return 0;
}
