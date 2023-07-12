#include <stdio.h>
#include "rocket.h"

int main()
{
    puts("CONTROL CALLING LUNAR MODULE. MANUAL CONTROL IS NECESSARY");
    puts("YOU MAY RESET FUEL RATE K EACH 10 SECS TO 0 OR ANY VALUE");
    puts("BETWEEN 8 & 200 LBS/SEC. YOU'VE 16000 LBS FUEL. ESTIMATED");
    puts("FREE FALL IMPACT TIME-120 SECS. CAPSULE WEIGHT-32500 LBS\n\n");

    do // 01.20 in original FOCAL code
    {
        puts("FIRST RADAR CHECK COMING UP\n\n");
        puts("COMMENCE LANDING PROCEDURE");
        puts("TIME,SECS   ALTITUDE,MILES+FEET   VELOCITY,MPH   FUEL,LBS   FUEL RATE");

        Rocket game;
        game.play_interactive();

        // On the Moon: 05.10 in original FOCAL code
        printf("ON THE MOON AT %8.2f SECS\n", game.get_seconds());
        double impact_velocity = 3600 * game.get_velocity();
        printf("IMPACT VELOCITY OF %8.2f M.P.H.\n", impact_velocity);
        printf("FUEL LEFT: %8.2f LBS\n", game.get_fuel());
        if (impact_velocity <= 1) {
            puts("PERFECT LANDING !-(LUCKY)");
        } else if (impact_velocity <= 10) {
            puts("GOOD LANDING-(COULD BE BETTER)");
        } else if (impact_velocity <= 22) {
            puts("CONGRATULATIONS ON A POOR LANDING");
        } else if (impact_velocity <= 40) {
            puts("CRAFT DAMAGE. GOOD LUCK");
        } else if (impact_velocity <= 60) {
            puts("CRASH LANDING-YOU'VE 5 HRS OXYGEN");
        } else {
            puts("SORRY,BUT THERE WERE NO SURVIVORS-YOU BLEW IT!");
            printf("IN FACT YOU BLASTED A NEW LUNAR CRATER %8.2f FT. DEEP\n", impact_velocity * .277777);
        }

        puts("\n\n\nTRY AGAIN?");
    } while (Rocket::accept_yes_or_no());

    puts("CONTROL OUT\n\n");
    return 0;
}
