//
// Filesystem Application Interface.
//
#ifndef FS_H
#define FS_H

#include <stdint.h>

// Type for file size in bytes.
typedef uint64_t fs_size_t;

//
// File function return codes.
//
typedef enum {
    FR_OK = 0,                   // Succeeded
    FR_DISK_ERR = 1,             // A hard error occurred in the low level disk I/O layer
    FR_INT_ERR = 2,              // Assertion failed
    FR_NOT_READY = 3,            // The physical drive cannot work
    FR_NO_FILE = 4,              // Could not find the file
    FR_NO_PATH = 5,              // Could not find the path
    FR_INVALID_NAME = 6,         // The path name format is invalid
    FR_DENIED = 7,               // Access denied due to prohibited access or directory full
    FR_EXIST = 8,                // Access denied due to prohibited access
    FR_INVALID_OBJECT = 9,       // The file/directory object is invalid
    FR_WRITE_PROTECTED = 10,     // The physical drive is write protected
    FR_INVALID_DRIVE = 11,       // The logical drive number is invalid
    FR_NOT_ENABLED = 12,         // The volume has no work area
    FR_NO_FILESYSTEM = 13,       // There is no valid FAT volume
    FR_MKFS_ABORTED = 14,        // The f_mkfs() aborted due to any problem
    FR_TIMEOUT = 15,             // Could not get a grant to access the volume within defined period
    FR_LOCKED = 16,              // The operation is rejected according to the file sharing policy
    FR_NOT_ENOUGH_CORE = 17,     // LFN working buffer could not be allocated
    FR_TOO_MANY_OPEN_FILES = 18, // Too many open files
    FR_INVALID_PARAMETER = 19,   // Given parameter is invalid
} fs_result_t;

//
// File access modes.
// Use as 3rd argument of f_open().
//
enum {
    FA_READ = 0x01,
    FA_WRITE = 0x02,
    FA_OPEN_EXISTING = 0x00,
    FA_CREATE_NEW = 0x04,
    FA_CREATE_ALWAYS = 0x08,
    FA_OPEN_ALWAYS = 0x10,
    FA_OPEN_APPEND = 0x30,
};

//
// File information structure.
//
#define FF_SFN_BUF 12             // Size of short file name
#define FF_LFN_BUF 255            // Size of long file name
typedef struct {
    fs_size_t fsize;              // File size
    uint16_t fdate;               // Modified date
    uint16_t ftime;               // Modified time
    uint8_t fattrib;              // File attribute
    char altname[FF_SFN_BUF + 1]; // Alternative file name
    char fname[FF_LFN_BUF + 1];   // Primary file name
} file_info_t;

// File attribute bits (file_info_t.fattrib)
enum {
    AM_RDO = 0x01, // Read only
    AM_HID = 0x02, // Hidden
    AM_SYS = 0x04, // System
    AM_DIR = 0x10, // Directory
    AM_ARC = 0x20, // Archive
};

//
// Format options (2nd argument of f_mkfs)
//
enum {
    FM_FAT = 0x01,   // Create FAT16 volume
    FM_FAT32 = 0x02, // Create FAT32 volume
    FM_EXFAT = 0x04, // Create exFAT volume
    FM_ANY = 0x07,   // Any of the above, depends on the volume size
    FM_SFD = 0x08,   // Non-partitioned disk format (Super-Floppy Disk)
};

//---------------------------------------------------------------------
// File access functions.
//
typedef struct _file_t file_t; // Opaque pointer
unsigned f_sizeof_file_t();

// Open or create a file.
fs_result_t f_open(file_t *fp, const char *path, uint8_t mode);

// Close an open file object.
fs_result_t f_close(file_t *fp);

// Read data from the file.
fs_result_t f_read(file_t *fp, void *buff, unsigned num_bytes_to_read,
                   unsigned *num_bytes_read);

// Write data to the file.
fs_result_t f_write(file_t *fp, const void *buff, unsigned num_bytes_to_write,
                    unsigned *num_bytes_written);

// Move file pointer of the file object.
fs_result_t f_lseek(file_t *fp, fs_size_t ofs);

// Truncate the file.
fs_result_t f_truncate(file_t *fp);

// Flush cached data of the writing file.
fs_result_t f_sync(file_t *fp);

// Forward data to the stream.
fs_result_t f_forward(file_t *fp, unsigned (*func)(const uint8_t *, unsigned),
                      unsigned num_bytes_to_forward, unsigned *num_bytes_forwarded);

// Allocate a contiguous block to the file.
fs_result_t f_expand(file_t *fp, fs_size_t fsz, uint8_t opt);

// Get a string from the file.
char *f_gets(char *buff, int len, file_t *fp);

// Put a character to the file.
int f_putc(char c, file_t *fp);

// Put a string to the file.
int f_puts(const char *str, file_t *cp);

// Put a formatted string to the file.
int f_printf(file_t *fp, const char *str, ...);

// Get file size in bytes.
fs_size_t f_size(file_t *fp);

// Get the current file position.
fs_size_t f_tell(file_t *fp);

// Return nonzero if the end-of-file indicator is set.
int f_eof(file_t *fp);

// Return nonzero if the error indicator is set.
int f_error(file_t *fp);

// Set the file position to the beginning of the file.
#define f_rewind(fp) f_lseek((fp), 0)

//---------------------------------------------------------------------
// Directory access functions.
//
typedef struct _directory_t directory_t; // Opaque pointer
unsigned f_sizeof_directory_t();

// Open a directory.
fs_result_t f_opendir(directory_t *dp, const char *path);

// Close an open directory.
fs_result_t f_closedir(directory_t *dp);

// Read a directory item.
fs_result_t f_readdir(directory_t *dp, file_info_t *fno);

// Find first file.
fs_result_t f_findfirst(directory_t *dp, file_info_t *fno, const char *path, const char *pattern);

// Find next file.
fs_result_t f_findnext(directory_t *dp, file_info_t *fno);

// Reset the position of the directory to the beginning of the directory.
#define f_rewinddir(dp) f_readdir((dp), 0)

//---------------------------------------------------------------------
// File and directory management functions.
//

// Get file status.
fs_result_t f_stat(const char *path, file_info_t *fno);

// Delete an existing file or directory.
fs_result_t f_unlink(const char *path);

// Rename/Move a file or directory.
fs_result_t f_rename(const char *path_old, const char *path_new);

// Change attribute of a file/dir.
fs_result_t f_chmod(const char *path, uint8_t attr, uint8_t mask);

// Change timestamp of a file/dir.
fs_result_t f_utime(const char *path, const file_info_t *fno);

// Create a sub directory.
fs_result_t f_mkdir(const char *path);

// Change current directory.
fs_result_t f_chdir(const char *path);

// Change current drive.
fs_result_t f_chdrive(const char *path);

// Get current directory.
fs_result_t f_getcwd(char *buff, unsigned len);

// Get the error message string corresponding to an error number.
const char *f_strerror(fs_result_t errnum);

//---------------------------------------------------------------------
// Volume management functions.
//
typedef struct _filesystem_t filesystem_t; // Opaque pointer
unsigned f_sizeof_filesystem_t();

// Mount/Unmount a logical drive.
fs_result_t f_mount(filesystem_t *fs, const char *path, uint8_t opt);

// Get number of free clusters on the drive.
fs_result_t f_getfree(const char *path, uint32_t *nclst, filesystem_t **fatfs);

// Get volume label.
fs_result_t f_getlabel(const char *path, char *label, uint32_t *vsn);

// Set volume label.
fs_result_t f_setlabel(const char *label);

// Create a FAT volume.
fs_result_t f_mkfs(const char *path, uint8_t fmt, void *work, unsigned len);

// Unmount a logical drive.
#define f_unmount(path) f_mount(0, path, 0)

#endif // FS_H
