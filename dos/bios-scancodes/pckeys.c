/*
 * pckeys.c - Read keyboard via Int 16h, print ASCII/scancode and shift flags.
 * Output to stdout and key.log. Exit on three Esc presses in a row (scancode 0x01).
 * For Borland Turbo C++ 3.0.
 */
#include <stdio.h>
#include <stdlib.h>

#define ESC_SCANCODE 0x01

static void print_shift_flags(unsigned char shift_flags, unsigned char ext_flags2,
                              char *buf_shift, char *buf_ext)
{
    /* Shift flags (AL from Int 16h AH=02h): bit0=RShift, 1=LShift, 2=Ctrl, 3=Alt,
       4=ScrollLock, 5=NumLock, 6=CapsLock, 7=Insert */
    buf_shift[0] = '\0';
    if (shift_flags & 0x01) strcat(buf_shift, "RShift ");
    if (shift_flags & 0x02) strcat(buf_shift, "LShift ");
    if (shift_flags & 0x04) strcat(buf_shift, "Ctrl ");
    if (shift_flags & 0x08) strcat(buf_shift, "Alt ");
    if (shift_flags & 0x10) strcat(buf_shift, "Scroll ");
    if (shift_flags & 0x20) strcat(buf_shift, "NumLock ");
    if (shift_flags & 0x40) strcat(buf_shift, "CapsLock ");
    if (shift_flags & 0x80) strcat(buf_shift, "Insert ");

    /* Extended flags (AH from Int 16h AH=12h): bit0=LCtrl, 1=LAlt, 2=RCtrl, 3=RAlt,
       4=Scroll, 5=NumLock, 6=CapsLock, 7=SysReq */
    buf_ext[0] = '\0';
    if (ext_flags2 & 0x01) strcat(buf_ext, "LCtrl ");
    if (ext_flags2 & 0x02) strcat(buf_ext, "LAlt ");
    if (ext_flags2 & 0x04) strcat(buf_ext, "RCtrl ");
    if (ext_flags2 & 0x08) strcat(buf_ext, "RAlt ");
    if (ext_flags2 & 0x10) strcat(buf_ext, "Scroll ");
    if (ext_flags2 & 0x20) strcat(buf_ext, "NumLock ");
    if (ext_flags2 & 0x40) strcat(buf_ext, "CapsLock ");
    if (ext_flags2 & 0x80) strcat(buf_ext, "SysReq ");
}

void print_result(FILE *out, unsigned ascii, unsigned scancode, const char *buf_shift, const char *buf_ext)
{
    if (ascii == 0) {
        fprintf(out, "   ");
    } else if (ascii < ' ') {
        fprintf(out, "^%c ", ascii + '@');
    } else if (ascii <= '~') {
        fprintf(out, " %c ", ascii);
    } else {
        fprintf(out, "   ");
    }
    fprintf(out, "ascii=%02X scancode=%02X shift: %s\n", ascii, scancode, buf_shift, buf_ext);
    fprintf(out, "                        ext: %s\n", buf_ext);
}

int main(void)
{
    unsigned char ascii, scancode, shift_flags, ext_flags1, ext_flags2;
    FILE *logfile;
    char buf_shift[80];
    char buf_ext[80];
    int esc_count = 0;

    logfile = fopen("key.log", "a");
    if (logfile == NULL) {
        fprintf(stderr, "Cannot open key.log for append\n");
    }

    printf("Keyboard logger: press keys (Esc 3x in a row to exit).\n");
    if (logfile != NULL) {
        fprintf(logfile, "Keyboard logger: press keys (Esc 3x in a row to exit).\n");
    }

    for (;;) {
        /* Int 16h AH=00h: wait for key, return AH=scancode, AL=ASCII */
        asm {
            mov ah, 0x00
            int 0x16
            mov scancode, ah
            mov ascii, al
        }

        if (scancode == ESC_SCANCODE) {
            esc_count++;
            if (esc_count >= 3) {
                break;
            }
        } else {
            esc_count = 0;
        }

        /* Int 16h AH=02h: get shift flags, return AL=shift flags */
        asm {
            mov ah, 0x02
            int 0x16
            mov shift_flags, al
        }

        /* Int 16h AH=12h: get extended shift states, AL=flags1, AH=flags2 */
        asm {
            mov ah, 0x12
            int 0x16
            mov ext_flags1, al
            mov ext_flags2, ah
        }

        print_shift_flags(shift_flags, ext_flags2, buf_shift, buf_ext);

        print_result(stdout, ascii, scancode, buf_shift, buf_ext);
        if (logfile != NULL) {
            print_result(logfile, ascii, scancode, buf_shift, buf_ext);
            fflush(logfile);
        }
    }

    if (logfile != NULL) {
        fclose(logfile);
    }
    return 0;
}
