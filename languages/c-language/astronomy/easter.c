//
// Calculate Easter date for a year.
//
// Serge Vakulenko
//
#include <stdio.h>
#include <stdlib.h>
#include "astronomy.h"

static int day_of_week(int year, int month, int day)
{
    static const int offset[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    year -= (month < 3);
    return (year + year/4 - year/100 + year/400 + offset[month - 1] + day) % 7;
}

int main(int argc, const char *argv[])
{
    int year;

    if (argc == 1) {
        astro_time_t time = Astronomy_CurrentTime();
        astro_utc_t utc = Astronomy_UtcFromTime(time);
        year = utc.year;
    } else if (argc == 2) {
        if (1 != sscanf(argv[1], "%d", &year)) {
            fprintf(stderr, "Invalid year '%s'.\n", argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr, "Usage: easter [year]\n");
        exit(1);
    }

    //
    // Find March equinox.
    //
    astro_search_result_t march_equinox = Astronomy_SearchSunLongitude(0, Astronomy_MakeTime(year, 3, 10, 0, 0, 0.0), 20.0);
    if (march_equinox.status != ASTRO_SUCCESS) {
        fprintf(stderr, "Cannot find March equinox.\n");
        exit(1);
    }
    astro_utc_t utc = Astronomy_UtcFromTime(march_equinox.time);
    printf("March equinox: %04d-%02d-%02d %02d:%02d UTC\n",
           utc.year, utc.month, utc.day, utc.hour, utc.minute);

    //
    // Find full moon.
    //
    astro_search_result_t full_moon = Astronomy_SearchMoonPhase(180.0, march_equinox.time, 30.0);
    if (full_moon.status != ASTRO_SUCCESS) {
        fprintf(stderr, "Cannot find full moon.\n");
        exit(1);
    }

    // Find day of the week.
    utc = Astronomy_UtcFromTime(full_moon.time);
    int dow = day_of_week(utc.year, utc.month, utc.day);
    static const char *weekday_name[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
    };
    printf("    Full Moon: %04d-%02d-%02d %02d:%02d UTC, %s\n",
           utc.year, utc.month, utc.day, utc.hour, utc.minute, weekday_name[dow]);

    //
    // Find next Sunday.
    //
    astro_time_t easter = Astronomy_AddDays(full_moon.time, 7 - dow);
    utc = Astronomy_UtcFromTime(easter);
    printf("       Easter: %04d-%02d-%02d\n", utc.year, utc.month, utc.day);
}
