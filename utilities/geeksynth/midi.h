extern struct midi_state_t {
    /*
     * State of "Program" knob.
     */
    unsigned char knob_prog;

    /*
     * State of "Master Volume" slider.
     */
    unsigned char slider_volume;

    /*
     * State of "Bank" switch, 0...3.
     */
    unsigned char switch_bank;

    /*
     * State of input midi controls.
     */
    unsigned char control[128];

#define BUTTON_MAIN_1       1
#define BUTTON_MAIN_2       2

#define SLIDER_1            3
#define SLIDER_2            4
#define SLIDER_3            5
#define SLIDER_4            6
#define SLIDER_5            7
#define SLIDER_6            8
#define SLIDER_7            9
#define SLIDER_8            10
#define SLIDER_V            11

#define KNOB_1              14
#define KNOB_2              15
#define KNOB_3              16
#define KNOB_4              17
#define KNOB_5              18
#define KNOB_6              19
#define KNOB_7              20
#define KNOB_8              21
#define KNOB_V              22

#define BUTTON_1            23
#define BUTTON_2            24
#define BUTTON_3            25
#define BUTTON_4            26
#define BUTTON_5            27
#define BUTTON_6            28
#define BUTTON_7            29
#define BUTTON_8            30
#define BUTTON_V            31

#define BUTTON_LOOP         49
#define BUTTON_BACK         47
#define BUTTON_FORW         48
#define BUTTON_STOP         46
#define BUTTON_PLAY         45
#define BUTTON_REC          44

#define BUTTON_PROG_LEFT    67
#define BUTTON_PROG_RIGHT   64

#define VALUE_MAX           127

} midi;

void midi_init(void);
