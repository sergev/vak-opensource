/*
 * Iambic keyer for arduino by Dimitris Sapountzakis (01/12/2011)
 */
#define DIT_PIN  8
#define DAH_PIN  10
#define EXC_PIN  9
#define LED      13

#define BAUD_DURATION          80               // mSec
#define INTERBAUD_DURATION     BAUD_DURATION*1
#define INTERLETTER_DURATION   BAUD_DURATION*2  // extra time after a baud
#define DIT_DURATION           BAUD_DURATION
#define DAH_DURATION           BAUD_DURATION*3
#define TOUCH_THRESHOLD        10               // how long to wait in uSec, before sampling the touch pin.

enum {
    IDLE,
    DIT,
    DAH,
};

int state;

int read_paddle (int pin)
{
    int active;

    digitalWrite (EXC_PIN, HIGH);
    delayMicroseconds (TOUCH_THRESHOLD);
    active = !digitalRead (pin);
    digitalWrite (EXC_PIN, LOW);
    return active;
}

void setup()
{
    pinMode (EXC_PIN, OUTPUT);
    digitalWrite (EXC_PIN, LOW);
    pinMode (LED, OUTPUT);
    digitalWrite (LED, LOW);            // turn off led
    state = IDLE;
}

//
// handles the output of the ciruit.
// if active is 1 then the contact is closed or led is turned on
//
void contact (int active)
{
    if (active) {
        digitalWrite (LED, HIGH);
        analogWrite (3, 127);           // pin 3 drives an 8 Ohm speaker
    } else {
        digitalWrite (LED, LOW);
        analogWrite (3, 0);
    }
}

void loop()
{
    switch (state) {
    case IDLE:
        if (read_paddle (DIT_PIN)) {
            state = DIT;
        } else {
            delayMicroseconds (30);
            if (read_paddle (DAH_PIN)) {
                state = DAH;
            }
        }
        break;

    case DIT:
        contact (1);
        delay (DIT_DURATION);
        contact (0);
        delay (INTERBAUD_DURATION);

        // now, if dah is pressed go there, else check for dit
        if (read_paddle (DAH_PIN)) {
            state = DAH;
        } else if (read_paddle (DIT_PIN)) {
            state = DIT;
        } else {
            delay (INTERLETTER_DURATION);
            state = IDLE;
        }
        break;

    case DAH:
        contact (1);
        delay (DAH_DURATION);
        contact (0);
        delay (INTERBAUD_DURATION);

        // now, if dit is pressed go there, else check for dah
        if (read_paddle (DIT_PIN)) {
            state = DIT;
        } else if (read_paddle (DAH_PIN)) {
            state = DAH;
        } else {
            delay (INTERLETTER_DURATION);
            state = IDLE;
        }
        break;
    }
    delay (1);
}
