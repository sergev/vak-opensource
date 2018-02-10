/*
 * Read SCP image format.
 *
 * Copyright (C) 2018 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __SCP_H__
#define __SCP_H__

#include <stdint.h>

//
// Disk Image Header
//
typedef struct {
    // BYTES 0x00-0x02 contains the ASCII of "SCP" as the first 3 bytes. If this is not found,
    // then the file is not ours.
    uint8_t sig[3];

    // BYTE 0x03 is the version/revision as a byte.  This is encoded as (Version<<4|Revision),
    // so that 0x39= version 3.9 of the format.  This is the version number of the SCP imaging
    // software that created this image.  If bit 5 (FOOTER) of the FLAGS (byte 0x08) is set,
    // this byte will be zero, and you are required to use the version and name entries in the
    // extension footer.
    uint8_t version;

    // BYTE 0x04 is the disk type and represents the type of disk for the image (see disk types
    // in the defines).
    uint8_t disk_type;

    // BYTE 0x05 is the number of revolutions, which is how many revolutions for each track is
    // contained in the image.
    uint8_t nr_revolutions;

    // Typically, a maximum of five sets of three longwords are stored
    // using the SuperCard Pro's imaging program.
#define REV_MAX 5

    // BYTES 0x06 and 0x07 are the start track and end track bytes.  Tracks are numbered 0-165,
    // which is a maximum of 166 tracks (83 tracks with top/bottom).
    uint8_t start_track;
    uint8_t end_track;

    // BYTE 0x08 is the FLAGS byte.  This byte contains information about how the image was
    // produced.
    //
    // FLAGS bit 0 is used to determine when the reading of flux data started.  If this bit is
    // set then the flux data was read immediately after the index pulse was detected.  If
    // this bit is clear then the flux data was read starting at some random location on the
    // track.
    //
    // FLAGS bit 1 is used for determining the type of 5.25" drive was used.  Does not apply
    // to any 3.5" drives.
    //
    // FLAGS bit 2 is used to determine the approximate RPM of the drive.  When FLAGS bit 0 is
    // clear, the index pulse is simulated using either a 166.6667ms (360 RPM) or 200ms (300 RPM)
    // index pulse.
    //
    // FLAGS bit 3 is used to determine if the image was made with the full resolution possible
    // or if the image quality was reduced using a normalization routine that is designed to
    // reduce the file size when compressed.
    //
    // FLAGS bit 4 is used to determine if the image is read-only or read/write capable.  Most
    // images will be read-only (write protected) for emulation usage.  The read/write capable
    // images contain padded space to allow the track to change size within the image.  Only a
    // single revolution is allowed when the TYPE bit is set (read/write capable).
    //
    // FLAGS bit 5 is used to determine the presence of an extension footer after the end of
    // the image.
    uint8_t flags;

#define FLAG_INDEX  0x01    // Bit 0 - cleared if the image did not use the index mark for queuing tracks
                            //         set is the image used the index mark to queue tracks
#define FLAG_TPI    0x02    // Bit 1 - cleared if the drive is a 48TPI drive
                            //         set if the drive is a 96TPI drive
#define FLAG_RPM    0x04    // Bit 2 - cleared if the drive is a 300 RPM drive
                            //         set if the drive is a 360 RPM drive
#define FLAG_TYPE   0x08    // Bit 3 - cleared for preservation quality image
                            //         set if flux has been normalized, reducing quality
#define FLAG_MODE   0x10    // Bit 4 - cleared if the image is read-only
                            //         set if the image is read/write capable
#define FLAG_FOOTER 0x20    // Bit 5 - cleared if the image does not contain an extension footer
                            //         set if the image contains an extension footer

    // BYTE 0x09 is the width of the bit cell time.  Normally this is always 0 which means
    // 16 bits wide, but if the value is non-zero then it represents the number of bits for
    // each bit cell entry.  For example, if this byte was set to 8, then each bit cell entry
    // would be 8 bits wide, not the normal 16 bits.  This is for future expansion, and may never
    // be actually used.
    uint8_t cell_width;

    // BYTE 0x0A is the head number(s) contained in the image.  This value is either 0, 1 or 2.
    // If the value is 0 then both heads are contained in the image, which has always been the
    // default for all SCP images (except C64).  A value of 1 means just side 0 (bottom) is
    // contained in the image, and a value of 2 means just side 1 (top) is contained in the image.
    uint8_t sides;

#define SIDE_BOTH   0
#define SIDE_BOTTOM 1
#define SIDE_TOP    2

    // BYTE 0x0B is reserved for future use.
    uint8_t reserved;

    // BYTES 0x0C-0x0F are the 32 bit checksum of data starting from offset 0x10 through the
    // end of the image file.  Checksum is standard addition, with a wrap beyond 32 bits
    // rolling over.  A value of 0x00000000 is used when FLAGS bit 4 (MODE) is set, as no checksum
    // is calculated for read/write capable images.
    uint32_t checksum;

    // BYTES 0x10-0x2AF are a table of longwords with each entry being a offset to a Track Data
    // Header (TDH) for each track that is stored in the image.  The table is always sequential.
    // There is an entry for every track, with up to 168 tracks supported.  This means that disk
    // images of up to 84 tracks with sides 0/1 are possible.  If no flux data for a track is
    // present, then the entry will contain a longword of zeros (0x00000000).  The 1st TDH
    // will probably start at offset 0x000002B0, but could technically start anywhere in the file
    // because all entries are offset based, so track data does not have to be in any order.  This
    // was done so you could append track data to the file and change the header to point to the
    // appended data.  For simplicity it is recommended that you follow the normal image format
    // shown below, with all tracks in sequential order.  The SuperCard Pro imaging software will
    // always create a disk with all tracks and revolutions stored sequentially.
#define TRACK_MAX 168
    uint32_t track_offset[TRACK_MAX];

} scp_disk_header_t;

//
// Track Header
//
typedef struct {
    // BYTES 0x00-0x02 contains the ASCII of "TRK" as the first 3 bytes.
    uint8_t sig[3];

    // BYTE 0x03 contains the track number (0-165).  For single sided disks, tracks 0-42 (48TPI)
    // or 0-82 (96TPI) are used.  For double-sided disks, the actual track number is this value
    // divided by 2.  The remainder (0 or 1) is the head.  0 represents the bottom head and
    // 1 represents the top head.  For example, 0x0B would be 11/2 = track 5, head 1 (top).
    // Likewise, 0x50 would be 80/2 = track 40, head 0 (bottom).
    uint8_t track_nr;

    // Starting at BYTE 0x04 are three longwords for each revolution that is stored.
    struct {
        // BYTES 0x04-0x07 contain the duration of the 1st revolution.  This is the time from index
        // pulse to index pulse.  This will be either ~360RPMs or ~300 RPMs depending the drive.
        // It is important to have this exact time as it is necessary when writing an image back to a
        // real disk.  The index time is a value in nanoseconds/25ns for one revolution.  Multiply
        // the value by 25 to get the exact time value in nanoseconds.
        uint32_t duration_25ns;

        // BYTES 0x08-0x0B contain the length of the track in bitcells (flux transitions).  If a
        // 16 bit width is used (selected above), then this value *2 will be the total number of
        // bytes of data that is used for that track (16 bit = 2 bytes per word).
        uint32_t nr_samples;

        // BYTES 0x0C-0x0F contains the offset from the start of the Track Data Header.
        // NOTE!!  THIS OFFSET IS *NOT* FROM THE START OF THE FILE!
        uint32_t offset;

    } rev[REV_MAX];

} scp_track_header_t;

typedef struct {
    int fd;
    scp_disk_header_t header;           /* disk image header */
    scp_track_header_t track;           /* current track header */


    /* Raw track data. */
    uint16_t *dat;
    unsigned int datsz;

    unsigned int index_ptr[REV_MAX];    /* data offsets of each index */
    unsigned int iter_ptr;              /* current index into dat[] */
    unsigned int iter_limit;            /* next index offset */

} scp_file_t;

int scp_open(scp_file_t *sf, const char *name);
void scp_close(scp_file_t *sf);
int scp_select_track(scp_file_t *sf, unsigned int tracknr);
void scp_reset(scp_file_t *sf);
unsigned scp_next_flux(scp_file_t *sf, unsigned int data_rpm);
void scp_print_disk_header(scp_file_t *sf);
void scp_print_track(scp_file_t *sf);
void scp_generate_vcd(scp_file_t *sf, const char *name);

#endif /* __SCP_H__ */
