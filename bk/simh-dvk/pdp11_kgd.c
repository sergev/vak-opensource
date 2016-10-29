/*
 * KGD: floppy disk controller for DVK
 *
 * Copyright (c) 2011, Serge Vakulenko
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#include "pdp11_defs.h"

extern uint16 *M;
extern int32 R[];
extern int32 int_req[];

#define NHEAD           4
#define NSECT           16
#define NCYL            612
#define KGD_SIZE        (NHEAD*NSECT*NCYL*512)  /* disk size, bytes */

/*
 * ERR register
 */
#define ERR_MARK        0x0100     /* no marker on read */
#define ERR_SEEKZ       0x0200     /* seek to zero failed */
#define ERR_HW          0x0400     /* hardware error */
#define ERR_ADDR        0x1000     /* sector not found */
#define ERR_ACRC        0x2000     /* address checksum error */
#define ERR_DCRC        0x4000     /* data checksum error */

/*
 * CS register
 */
#define CS_CMD_MASK     0x00ff      /* command mask */
#define CS_CMD_SEEKZ    0020        /* seek to track 0 */
#define CS_CMD_RD       0040        /* read */
#define CS_CMD_WR       0060        /* write */
#define CS_CMD_FORMAT   0120        /* select track */
#define CS_ERR          0x0100      /* nonzero ERR register */
#define CS_DR2          0x0800      /* data request 2 */
#define CS_INIT         0x1000      /* init done */
#define CS_WFLT         0x2000      /* write failed */
#define CS_READY        0x4000      /* disk ready */

/*
 * SI register
 */
#define SI_DONE         0x0001      /* command done */
#define SI_RESET        0x0008      /* reset device */
#define SI_INTE         0x0040      /* enable interrupt */
#define SI_DR1          0x0080      /* data request 1 */
#define SI_SLOW         0x0100      /* slow seek */
#define SI_BUSY         0x8000      /* device busy */

/*
 * Hardware registers.
 */
int kgd_id;             /* Identification */
int kgd_err;            /* Errors */
int kgd_sector;         /* Sector 0..15 */
int kgd_cyl;            /* Cylinder 0..611 */
int kgd_head;           /* Head 0..3 */
int kgd_cs;             /* Command and status */
int kgd_si;             /* Status and init */
int kgd_count;
uint16 kgd_buf [256];

t_stat kgd_event (UNIT *u);
t_stat kgd_rd (int32 *data, int32 PA, int32 access);
t_stat kgd_wr (int32 data, int32 PA, int32 access);
int32 kgd_inta (void);
t_stat kgd_reset (DEVICE *dptr);
t_stat kgd_boot (int32 unitno, DEVICE *dptr);
t_stat kgd_attach (UNIT *uptr, char *cptr);
t_stat kgd_detach (UNIT *uptr);

/*
 * KGD data structures
 *
 * kgd_unit     unit descriptors
 * kgd_reg      register list
 * kgd_dev      device descriptor
 */
UNIT kgd_unit [] = {
    { UDATA (kgd_event, UNIT_FIX+UNIT_ATTABLE+UNIT_ROABLE, KGD_SIZE) },
};

REG kgd_reg[] = {
    { "ID",     &kgd_id,     DEV_RDX, 16, 0, 1 },
    { "ERR",    &kgd_err,    DEV_RDX, 16, 0, 1 },
    { "SECTOR", &kgd_sector, DEV_RDX, 16, 0, 1 },
    { "CYL",    &kgd_cyl,    DEV_RDX, 16, 0, 1 },
    { "HEAD",   &kgd_head,   DEV_RDX, 16, 0, 1 },
    { "CS",     &kgd_cs,     DEV_RDX, 16, 0, 1 },
    { "SI",     &kgd_si,     DEV_RDX, 16, 0, 1 },
    { 0 }
};

MTAB kgd_mod[] = {
    { 0 }
};

DIB kgd_dib = {
    IOBA_KGD, IOLN_KGD, &kgd_rd, &kgd_wr,
    1, IVCL (KGD), VEC_KGD, { &kgd_inta }
};

DEVICE kgd_dev = {
    "KGD", kgd_unit, kgd_reg, kgd_mod,
    1,          /* #units */
    DEV_RDX,    /* address radix */
    T_ADDR_W,   /* address width */
    2,          /* addr increment */
    DEV_RDX,    /* data radix */
    16,         /* data width */
    NULL, NULL, &kgd_reset, &kgd_boot, &kgd_attach, &kgd_detach,
    &kgd_dib, DEV_DISABLE | DEV_UBUS | DEV_QBUS | DEV_DEBUG
};

/*
 * Output to console and log file: when enabled "cpu debug".
 * Appends newline.
 */
void kgd_debug (const char *fmt, ...)
{
    va_list args;
    extern FILE *sim_deb;

    va_start (args, fmt);
    vprintf (fmt, args);
    printf ("\r\n");
    fflush (stdout);
    va_end (args);
    if (sim_deb && sim_deb != stdout) {
        va_start (args, fmt);
        vfprintf (sim_deb, fmt, args);
        fprintf (sim_deb, "\n");
        fflush (sim_deb);
        va_end (args);
    }
}

/*
 * Reset routine
 */
t_stat kgd_reset (DEVICE *dptr)
{
    if (kgd_dev.dctrl)
        kgd_debug ("+++ DW reset");
    kgd_id = 0401;                  /* Identification */
    kgd_err = 0;                    /* Errors */
    kgd_sector = 0;                 /* Sector 0..15 */
    kgd_cyl = 0;                    /* Cylinder 0..611 */
    kgd_head = 0;                   /* Head 0..3 */
    kgd_cs = CS_INIT | CS_READY;    /* Command and status */
    kgd_si = SI_DONE | SI_INTE;     /* Status and init */
    sim_cancel (&kgd_unit[0]);
    return SCPE_OK;
}

t_stat kgd_attach (UNIT *u, char *cptr)
{
    t_stat s;

    s = attach_unit (u, cptr);
    if (s != SCPE_OK)
        return s;
    // TODO
    return SCPE_OK;
}

t_stat kgd_detach (UNIT *u)
{
    // TODO
    return detach_unit (u);
}

/*
 * Boot from given unit.
 */
t_stat kgd_boot (int32 unitno, DEVICE *dptr)
{
    UNIT *u = &kgd_unit [unitno];
    extern int32 saved_PC;

    /* Read 1 sector to address 0. */
    fseek (u->fileref, 512, SEEK_SET);
    if (sim_fread (&M[0], 1, 512, u->fileref) != 512)
        return SCPE_IOERR;

    /* Jump to 0. */
    saved_PC = 0;
    return SCPE_OK;
}

static const char *regname (int a)
{
    switch (a & 036) {
    case 000: return "ID";
    case 004: return "ERR";
    case 006: return "SECTOR";
    case 010: return "DATA";
    case 012: return "CYL";
    case 014: return "HEAD";
    case 016: return "CS";
    case 020: return "SI";
    }
    static char buf[16];
    sprintf (buf, "%06o", a);
    return buf;
}

/*
 * I/O dispatch routines
 */
t_stat kgd_rd (int32 *data, int32 PA, int32 access)
{
    switch (PA & 036) {
    case 000:                   /* ID */
        *data = kgd_id;
        kgd_si &= ~SI_DR1;
        break;
    case 004:                   /* ERR */
        *data = kgd_err;
        break;
    case 006:                   /* SECTOR */
        *data = kgd_sector;
        kgd_si &= ~SI_DONE;
        break;
    case 010:                   /* Data register */
        if ((kgd_cs & CS_CMD_MASK) != CS_CMD_RD)
            break;
        if (kgd_count < 256) {
            *data = kgd_buf [kgd_count++];
        }
        if (kgd_count == 256) {
            kgd_si |= SI_DONE;
            kgd_si &= ~SI_DR1;
            kgd_cs &= ~CS_DR2;
        }
        break;
    case 012:                   /* CYL */
        *data = kgd_cyl;
        break;
    case 014:                   /* HEAD */
        *data = kgd_head;
        break;
    case 016:                   /* CS */
        *data = kgd_cs;
        kgd_si &= ~SI_DONE;
        break;
    case 020:                   /* SI */
        *data = kgd_si;
        break;
    }
    //if (kgd_dev.dctrl)
    //    kgd_debug ("+++ DW %s -> %06o", regname (PA), *data);
    return SCPE_OK;
}

t_stat kgd_wr (int32 data, int32 PA, int32 access)
{
    unsigned long seek;

    //if (kgd_dev.dctrl)
    //    kgd_debug ("+++ DW %s := %06o", regname (PA), data);
    switch (PA & 036) {
    case 000:                   /* ID */
        kgd_si &= ~SI_DR1;
        break;
    case 004:                   /* ERR */
        /* Ignore precompensation. */
        break;
    case 006:                   /* SECTOR */
        kgd_sector = data & 31;
        kgd_si &= ~SI_DONE;
        break;
    case 010:                   /* Data register */
        if ((kgd_cs & CS_CMD_MASK) != CS_CMD_WR)
            break;
        if (kgd_count < 256) {
            kgd_buf [kgd_count++] = data;
        }
        if (kgd_count == 256) {
            seek = ((kgd_cyl * NHEAD + kgd_head) * NSECT + kgd_sector) * 512L;
            fseek (kgd_unit[0].fileref, seek, SEEK_SET);
            sim_fwrite (kgd_buf, 1, 512, kgd_unit[0].fileref);
            if (ferror (kgd_unit[0].fileref))
                printf ("+++ DW write error, chs=%d/%d/%d\n",
                    kgd_cyl, kgd_head, kgd_sector);
            kgd_si |= SI_DONE;
            kgd_si &= ~SI_DR1;
            kgd_cs &= ~CS_DR2;
        }
        break;
    case 012:                   /* CYL */
        kgd_cyl = data & 1023;
        break;
    case 014:                   /* HEAD */
        kgd_head = data & 7;
        break;
    case 016:                   /* CS */
        kgd_cs = (kgd_cs & ~CS_CMD_MASK) | (data & CS_CMD_MASK);
        kgd_si &= ~SI_DONE;
        kgd_err = 0;
        switch (kgd_cs & CS_CMD_MASK) {
        case CS_CMD_RD:
            if (kgd_dev.dctrl)
                kgd_debug ("+++ DW read chs=%d/%d/%d",
                    kgd_cyl, kgd_head, kgd_sector);
            seek = ((kgd_cyl * NHEAD + kgd_head) * NSECT + kgd_sector) * 512L;
            fseek (kgd_unit[0].fileref, seek, SEEK_SET);
            if (sim_fread (kgd_buf, 1, 512, kgd_unit[0].fileref) != 512) {
                /* Reading uninitialized media. */
                kgd_cs |= CS_ERR;
                kgd_err |= ERR_ADDR;
            }
            if (ferror (kgd_unit[0].fileref))
                printf ("+++ DW read error, chs=%d/%d/%d\n",
                    kgd_cyl, kgd_head, kgd_sector);
            kgd_count = 0;
            kgd_si |= SI_DR1;
            kgd_cs |= CS_DR2;
            break;
        case CS_CMD_WR:
            if (kgd_dev.dctrl)
                kgd_debug ("+++ DW write chs=%d/%d/%d",
                    kgd_cyl, kgd_head, kgd_sector);
            kgd_count = 0;
            break;
        default:
            if (kgd_dev.dctrl)
                kgd_debug ("+++ DW cmd %03o not implemented",
                    kgd_cs & CS_CMD_MASK);
            break;
        }
        break;
    case 020:                   /* SI */
        kgd_si = (kgd_si & (SI_DONE | SI_DR1 | SI_BUSY)) |
            (data & (SI_INTE | SI_SLOW));
        if (data & SI_RESET) {
            /* Reset comntroller. */
            kgd_reset (&kgd_dev);
        }
        break;
    }
    if ((kgd_si & SI_INTE) && (kgd_si & (SI_DONE | SI_DR1)))
        SET_INT (KGD);
    else
        CLR_INT (KGD);
    return SCPE_OK;
}

/*
 * Return interrupt vector
 */
int32 kgd_inta (void)
{
    if ((kgd_si & SI_INTE) && (kgd_si & (SI_DONE | SI_DR1))) {
        /* return vector */
        //if (kgd_dev.dctrl)
        //    kgd_debug ("+++ DW inta vector %06o", kgd_dib.vec);
        return kgd_dib.vec;
    }
    /* no intr req */
    if (kgd_dev.dctrl)
        kgd_debug ("+++ DW inta no IRQ");
    return 0;
}

/*
 * Event: delayed i/o finished.
 */
t_stat kgd_event (UNIT *u)
{
    // TODO
    return SCPE_OK;
}
