/*
 * Arduino Pong
 * A simple implementation of Pong on the Arduino using a TV for output.
 *
 * By Pete Lamonica
 * modified by duboisvb
 * updated by James Bruce (http://www.makeuseof.com/tag/author/jbruce
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/gpanel.h>

#define WHEEL_ONE_PIN       0 //analog
#define WHEEL_TWO_PIN       1 //analog
#define BUTTON_ONE_PIN      2 //digital to start game

#define PADDLE_HEIGHT       14
#define PADDLE_WIDTH        1
#define BALL_SIZE           4

#define RIGHT_PADDLE_X      (WIDTH - 4)
#define LEFT_PADDLE_X       2

#define IN_GAME             0 //in game state
#define IN_MENU             1 //in menu state
#define GAME_OVER           2 //game over state

#define MAX_Y_VELOCITY      6
#define PLAY_TO             7

#define WIDTH               320
#define HEIGHT              240

/*
 * Assign human-readable names to some common 16-bit color values:
 */
#define	BLACK   0x0000
#define	BLUE    0x002F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/*
 * Data from external font files.
 */
extern const struct gpanel_font_t font_lucidasans15;
extern const struct gpanel_font_t font_lucidasans28;

unsigned x, y;

int button_pressed = 0;

int wheelOnePosition = 0;
int wheelTwoPosition = 0;
int rightPaddleY = 0;
int leftPaddleY = 0;
unsigned ballX = 0;
unsigned ballY = 0;
char ballVolX = 3;
char ballVolY = 3;

int leftPlayerScore = 0;
int rightPlayerScore = 0;

int frame = 0;
int state = IN_MENU;

extern int get_input(void);

void processInputs()
{
    int input = get_input();

    if (input != 0)
        button_pressed = 1;

    if (input == 'w') {
        if (wheelOnePosition > 0)
            wheelOnePosition--;
    }
    if (input == 's') {
        if (wheelOnePosition < 7)
            wheelOnePosition++;
    }
    if (input == 'i') {
        if (wheelTwoPosition > 0)
            wheelTwoPosition--;
    }
    if (input == 'k') {
        if (wheelTwoPosition < 7)
            wheelTwoPosition++;
    }
    usleep(10000);
}

void drawGameScreen()
{
    int i;
    //gpanel_clear(BLACK, 0, 0);

    /* Draw right paddle. */
    rightPaddleY = wheelOnePosition * (HEIGHT-PADDLE_HEIGHT) / 8;
    x = RIGHT_PADDLE_X;
    for (i=0; i<PADDLE_WIDTH; i++) {
        gpanel_fill(WHITE, x+i, rightPaddleY, x+i, rightPaddleY+PADDLE_HEIGHT);
    }

    /* Draw left paddle. */
    leftPaddleY = wheelTwoPosition * (HEIGHT-PADDLE_HEIGHT) / 8;
    x = LEFT_PADDLE_X;
    for (i=0; i<PADDLE_WIDTH; i++) {
        gpanel_fill(WHITE, x+i, leftPaddleY, x+i, leftPaddleY+PADDLE_HEIGHT);
    }

    /* Draw score. */
    gpanel_char(&font_lucidasans28, WHITE, BLACK,
        WIDTH/2 - 8 - 25, 4, '0'+leftPlayerScore);
    gpanel_char(&font_lucidasans28, WHITE, BLACK,
        WIDTH/2 + 10, 4, '0'+rightPlayerScore);

    /* Draw ball. */
    gpanel_fill(YELLOW, ballX, ballY, ballX+BALL_SIZE-1, ballY+BALL_SIZE-1);
}

void playerScored()
{
    /* Sound. */
    usleep(100000);
    //tone(500, 300);

    /* Check for win. */
    if (leftPlayerScore == PLAY_TO || rightPlayerScore == PLAY_TO) {
        state = GAME_OVER;
    }

    ballVolX = -ballVolX;
}

void drawNet()
{
    int i;

    /* Draw net. */
    for (i=1; i<HEIGHT - 4; i+=6) {
        gpanel_fill(WHITE, WIDTH/2, i, WIDTH/2, i+3);
    }
}

void drawMenu()
{
    int dX = 2;
    int dY = 2;

    x = 0;
    y = 0;
    gpanel_clear(BLACK, 0, 0);
    gpanel_text(&font_lucidasans28, WHITE, BLACK, 20, 10, "Arduino Pong");
    gpanel_text(&font_lucidasans15, WHITE, BLACK, 45, 70, "Press Button");
    gpanel_text(&font_lucidasans15, WHITE, BLACK, 60, 90, "To Start");

    usleep(1000000);
    while (! button_pressed) {
        processInputs();

        if (x + dX < 1 || x + dX >= WIDTH - BALL_SIZE)
            dX = -dX;

        if (y + dY < 1 || y + dY >= HEIGHT - BALL_SIZE)
            dY = -dY;
#if 0
        if (gpanel_get_pixel(x + dX, y + dY) != BLACK) {
            gpanel_pixel(BLACK, x + dX, y + dY);

            if (gpanel_get_pixel(x + dX, y - dY) == BLACK) {
                dY = -dY;
            }
            else if (gpanel_get_pixel(x - dX, y + dY) == BLACK) {
                dX = -dX;
            } else {
                dX = -dX;
                dY = -dY;
            }
        }
#endif
        gpanel_fill(BLACK, x, y, x+BALL_SIZE-1, y+BALL_SIZE-1);
        x += dX;
        y += dY;
        gpanel_fill(YELLOW, x, y, x+BALL_SIZE-1, y+BALL_SIZE-1);
    }
    button_pressed = 0;
    state = IN_GAME;

    gpanel_clear(BLACK, 0, 0);
    drawNet();
}

int main()
{
    char *devname = "/dev/tft0";
    int xsize = WIDTH, ysize = HEIGHT;

    if (gpanel_open(devname) < 0) {
        printf("Cannot open %s\n", devname);
        exit(-1);
    }
    gpanel_clear(BLACK, &xsize, &ysize);

    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    for(;;) {
        processInputs();

        if (state == IN_MENU) {
            drawMenu();
        }

        if (state == IN_GAME) {
            if (frame % 3 == 0) { //every third frame
                /* Clear ball. */
                gpanel_fill(BLACK, ballX, ballY, ballX+BALL_SIZE-1, ballY+BALL_SIZE-1);

                ballX += ballVolX;
                ballY += ballVolY;

                if (ballVolY < 0 && ballY < -ballVolY) {
                    // change if hit top
                    ballVolY = -ballVolY;
                    usleep(100000);
                    //tone(2000, 30);
                }
                else if (ballVolY > 0 && ballY > HEIGHT-BALL_SIZE-ballVolY) {
                    // change if hit bottom
                    ballVolY = -ballVolY;
                    usleep(100000);
                    //tone(2000, 30);
                }

                // test left side for wall hit
                if (ballVolX < 0 &&
                    ballX <= LEFT_PADDLE_X + PADDLE_WIDTH - 1 &&
                    ballY >= leftPaddleY &&
                    ballY <= leftPaddleY + PADDLE_HEIGHT)
                {
                    ballVolX = -ballVolX;
                    ballVolY += 2 * ((ballY - leftPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
                    usleep(100000);
                    //tone(2000, 30);
                }

                // test right side for wall hit
                if (ballVolX > 0 &&
                    ballX+BALL_SIZE-1 >= RIGHT_PADDLE_X &&
                    ballY >= rightPaddleY &&
                    ballY <= rightPaddleY + PADDLE_HEIGHT)
                {
                    ballVolX = -ballVolX;
                    ballVolY += 2 * ((ballY - rightPaddleY) - (PADDLE_HEIGHT / 2)) / (PADDLE_HEIGHT / 2);
                    usleep(100000);
                    //tone(2000, 30);
                }

                // limit vertical speed
                if (ballVolY > MAX_Y_VELOCITY)
                    ballVolY = MAX_Y_VELOCITY;
                if (ballVolY < -MAX_Y_VELOCITY)
                    ballVolY = -MAX_Y_VELOCITY;

                // Scoring
                if (ballX <= 1) {
                    rightPlayerScore++;
                    playerScored();
                }
                if (ballX >= WIDTH - 1) {
                    leftPlayerScore++;
                    playerScored();
                }
            }

            drawGameScreen();
        }

        if (state == GAME_OVER) {
            drawGameScreen();
            gpanel_text(&font_lucidasans28, RED, BLACK, 29, 25, "GAME OVER");
            while (! button_pressed) {
                processInputs();
                usleep(50000);
            }
            button_pressed = 0;

            /* Reset the scores. */
            leftPlayerScore = 0;
            rightPlayerScore = 0;
            state = IN_MENU;
        }

        /* Increment and/or reset frame counter. */
        if (++frame == 60)
            frame = 0;
    }
}
