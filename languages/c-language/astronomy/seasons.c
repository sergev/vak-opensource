/*
    seasons.c  -  Don Cross  -  2019-06-16

    Example C program for Astronomy Engine:
    https://github.com/cosinekitty/astronomy

    This program demonstrates how to calculate the
    equinoxes and solstices for a year.
*/

#include <stdio.h>
#include "astro_demo_common.h"

void Print(const char *name, astro_time_t time)
{
    printf("%-17s : ", name);
    PrintTime(time);
    printf("\n");
}

int main(int argc, const char *argv[])
{
    int year;
    astro_seasons_t seasons;

    if (argc == 1)
    {
        astro_time_t time = Astronomy_CurrentTime();
        astro_utc_t utc = Astronomy_UtcFromTime(time);
        year = utc.year;
    }
    else if (argc == 2)
    {
        if (1 != sscanf(argv[1], "%d", &year))
        {
            fprintf(stderr, "Invalid year '%s'.\n", argv[1]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: seasons [year]\n");
        return 1;
    }

    seasons = Astronomy_Seasons(year);
    if (seasons.status != ASTRO_SUCCESS)
    {
        fprintf(stderr, "ERROR: Astronomy_Seasons() returned %d\n", seasons.status);
        return 1;
    }

    Print("March equinox", seasons.mar_equinox);
    Print("June solstice", seasons.jun_solstice);
    Print("September equinox", seasons.sep_equinox);
    Print("December solstice", seasons.dec_solstice);

    return 0;
}
